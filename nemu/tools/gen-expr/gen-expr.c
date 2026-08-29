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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"#include <stdint.h>\n"
"int main() { "
"  uint32_t result; "
"  result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static int c = 0;

static int gen_num(){
	if (c >= 65530) return -1;
	int n;
	n = sprintf(buf + c, "%d", rand() & 0xffff);
	assert(n>0);
	c += n;
	return 0;
}

static int gen_op(){
	if (c >= 65534) return -1;
	switch(rand() % 4){
		case 0:
			buf[c++] = '+';
			break;
		case 1:
			buf[c++] = '-';
			break;
		case 2:
			buf[c++] = '*';
			break;
		case 3:
			buf[c++] = '/';
			break;
		default:
			assert(0);
	}
	return 0;
}

static int gen_rand_expr() {
	// sometime extrmely long expression
	if (c >= 65530) return -1;
	if(rand()%3 == 0) buf[c++] = ' ';
	switch(rand()%3){
			case 0:
				if(gen_num()) return -1;
				break;
			case 1:
				buf[c++] = '(';
				if(gen_rand_expr()) return -1;
				buf[c++] = ')';
				break;
			case 2:
				if(gen_rand_expr()) return -1;
				if(gen_op()) return -1;
				if(gen_rand_expr()) return -1;
				break;
			default:
				assert(0);
	}
	if(rand()%3 == 0) buf[c++] = ' ';
	buf[c] = '\0';
	return 0;
}

int main(int argc, char *argv[]) {
	FILE *fp;
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
		c = 0;
    if(gen_rand_expr()) continue;

    sprintf(code_buf, code_format, buf);

    fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Werror -Wno-overflow /tmp/.code.c -o /tmp/.expr");
		if(ret == 256) {
			printf("1 1\n");
			continue;
		}

    int result;
		fp = popen("/tmp/.expr", "r");
		ret = fscanf(fp, "%d", &result);
		assert(ret == 1);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
