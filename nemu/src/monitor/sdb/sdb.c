/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

#include <memory/vaddr.h>
#include <time.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
	nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args){
	int n = 0;
	if(args) n = atoi(args);
	if (n <= 0) n = 1;
	cpu_exec(n);
	return 0;
}

static int cmd_info(char *args){
	if(!args) return 0;
	switch(args[0]){
		case 'r':
			isa_reg_display();
			break;
		case 'w':
			print_watchpoint();
			break;
		default:
			return 0;
	}
	return 0;
}

static int cmd_x(char *args){
	printf("warning: this may be wrong\n");
	char *temp;
	temp = strtok(args, " ");
	if(!temp) { printf("usage: x N expr\n"); return 0; }
	int n;
	n = strtol(temp, NULL, 10);

	temp = strtok(NULL, " ");
	if(!temp) { printf("usage: x N expr\n"); return 0; }

	vaddr_t addr;
	bool success;
	addr = expr(temp, &success);

	if(n <= 0 || n > 64){ 
		printf("invalid parameter N\n"); 
		return 0; 
	}
	if(!success){
		printf("invalid expr\n");
		return 0;
	}

	int i = 0, j = 0;
	while(1){
		printf("0x%08x:  ", addr + i * 4);
		j=0;
		while(j<4){
			printf("%08x  ", vaddr_read(addr + i * 4, 4));
			if(++i == n) { printf("\n"); return 0;}
			j++;
		}
		printf("\n");
	}
}

static int cmd_p(char *args){
	printf("warning: this may be wrong\n");
	if(!args){
		printf("no expression\n");
		return 0;
	}
	bool success;
	int64_t val = expr(args, &success);
	if(success) printf("%ld\n", val);
	else printf("evaluation fail\n");
	return 0;
}

static int cmd_w(char *args) {
	if(new_wp(args)) {
		printf("fail to create watchpoint\n");
		return -1;
	}
	printf("create watchpoint\n");
	return 0;
}

static int cmd_d(char *args) {
	if(args == NULL){
		printf("usage: d [id]\n");
		return 0;
	}
	if (!free_wp(atoi(args))) {
		printf("fail to delete watchpoint\n");
		return -1;
	}
	printf("delete watchpoint\n");
	return 0;
}

static int cmd_check_expr(char *args) {
	int n = 0;
	if(args) n = atoi(args);
	if(n <= 0) n = 1;
	uint32_t result, val;
	bool success;
	char text[65536 + 128];
	char *expression;
	FILE *fp, *lg;
	int i;
	lg = fopen("/home/hbzd/Desktop/expr.txt", "a+");
	assert(lg);
	for (; n>0; n--){
		int t = time(0);
		fprintf(lg, "seed: %d\n", t);
		fp = popen("/home/hbzd/ysyx-workbench/nemu/tools/gen-expr/gen-expr", "r");
		if (!fgets(text, 65536 + 128, fp)){
			printf("read expr error:%s\n", text);
			pclose(fp);
			while(t==time(0));
			break;
		}
		pclose(fp);
		fprintf(lg, "%s\n", text);
		result = strtol(text, &expression, 10);
		for(i=0;expression[i] != '\n' && expression[i] != '\0'; i++);
		expression[i] = '\0';
		val = expr(expression, &success);
		if(val != result || !success){
			printf("wrong result:%d correct result:%d\ntext:%s\nexpression:%s\n", val, result, text, expression);
			while(t==time(0));
			continue;
		}
		while(t==time(0));
	}
	fclose(lg);
	printf("finish\n");
	return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{ "si", "Execute 1 or more instructions", cmd_si },
	{ "info", "print register state (r) or watchpoint info (w)", cmd_info },
	{ "x", "print data in memory", cmd_x },
	{ "p", "evaluate expression", cmd_p },
	{ "check-expr", "check expression evaluation", cmd_check_expr },
	{ "w", "create watchpoint", cmd_w },
	{ "d", "delete watchpoint", cmd_d },

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
