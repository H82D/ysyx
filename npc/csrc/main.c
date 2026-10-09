#include "common.h"

FILE* itrace_fp = NULL;
TOP_NAME dut;

// lllllllllllllllllllll
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
	strncpy(itrace_file, strcat(work_dir, "riscv32e-npc-itrace.txt"), 1000);
	itrace_fp = fopen(itrace_file, "w+");

	difftest_init("/home/hbzd/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so");

	monitor_init();

	if (CPU_STATUS == END)
		return 0;
	else 
		return -1;
}
