#include "common.h"

FILE* itrace_fp = NULL;
TOP_NAME dut;
extern bool batch_mode;

//void nvboard_bind_all_pins(TOP_NAME *top);

int main(int argc, char *argv[]){

	//nvboard_bind_all_pins(&dut);
	nvboard_init();

	char work_dir[1000];
	if (argc == 1) {
		printf("no image given\n");
		return -1;
	}
	else { 
		strncpy(work_dir, argv[1], 1000);
		char *p = strstr(work_dir, "build/");
		assert(p);
		p[6] = 0;
		ram_init(argv[1]);
	}

	cpu_init();

	char itrace_file[1000];
	strncpy(itrace_file, work_dir, 1000);
	strcat(itrace_file, "riscv32e-npc-itrace.txt");
	itrace_fp = fopen(itrace_file, "w+");


	difftest_init("/home/hbzd/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so");

	if (strcmp(argv[2], "-b") == 0) {
		batch_mode = 1;
	} else {
		batch_mode = 0;
	}
	monitor_init();

	if (CPU_STATUS == END)
		return 0;
	else 
		return -1;
}
