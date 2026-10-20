#include "common.h"
#include <stdio.h>

bool batch_mode;
const char *reg_names[32] = {
	"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
	"s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
};

static int cmd_c (char *args) {
	cpu_cycle(-1);
	return 0;
}

static int cmd_q (char *args) {
	CPU_STATUS = EXCEPT;
	return -1;
}

static int cmd_si (char *args) {
	int n = 0;
	if (args) n = atoi(args);
	cpu_cycle(n);
	return 0;
}

static int cmd_info (char *args) {
	printf("pc = 0x%08x\n", dut.pc_copy);
	for(int i = 0; i < 16; i++) {
		printf("x%-2d: %s = %-10d 0x%-8x\n", i, reg_names[i], dut.reg_copy[i], dut.reg_copy[i]);
	}
	printf("\n");
	return 0;
}

static int cmd_x (char *args) {
	printf("Not implemented\n");
	return 0;
}

static int cmd_p (char *args) {
	printf("Not implemented\n");
	return 0;
}

static int cmd_w (char *args) {
	printf("Not implemented\n");
	return 0;
}

static int cmd_d (char *args) {
	printf("Not implemented\n");
	return 0;
}

static struct {
	const char *name;
	int (*handler)(char*);
} cmd_table[] = {
	{"c", cmd_c},
	{"q", cmd_q},
	{"si", cmd_si},
	{"info", cmd_info},
	{"x", cmd_x},
	{"p", cmd_p},
	{"w", cmd_w},
	{"d", cmd_d},
};
#define NR_CMD 7

void monitor_init() {
	if (batch_mode)
		cpu_cycle(-1);
	else {
		char ins[128];
		while (1) {
			printf("(npc)");
			if (!fgets(ins, 128, stdin)) continue;
			for (int i = 0; ins[i] != '\n' && ins[i] != 0; i++){
				if (ins[i] == '\n') {
					ins[i] = 0;
					break;
				}
			}
			char *str_end = ins + strlen(ins);
	
			char *cmd = strtok(ins, " ");
			if (cmd == NULL) continue;
	
			char *args = cmd + strlen(cmd) + 1;
			if (args >= str_end) args = NULL;
	
			int i;
			for (i = 0; i < NR_CMD; i++) {
				if (strcmp(cmd, cmd_table[i].name) == 0) {
					if(cmd_table[i].handler(args) < 0) return;
					break;
				}
			}
	
			if (i == NR_CMD) printf("Unknown command '%s'\n", cmd);
		}
	}
}


