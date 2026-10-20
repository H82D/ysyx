#include "common.h"

CPU_status CPU_STATUS;
static bool print_step;

extern "C" int inv_inst() {
	printf("Invalid instruction %08x at 0x%08x\n", ram_read(1, dut.pc_copy), dut.pc_copy);
	CPU_STATUS = EXCEPT;
	return -1;
}

extern "C" int ebreak() {
	CPU_STATUS = EBREAK;
	return 0;
}

static void itrace(uint32_t addr) {
	uint32_t inst = ram_read(1, addr);
	fprintf(itrace_fp, "0x%08x: %08x\n", addr, inst);
	fflush(itrace_fp);
	if (print_step)
		printf("0x%08x: %08x\n", addr, inst);
}

static void single_step() {
	CPU_STATUS = RUNNING;
	dut.clk = 0;
	dut.eval();
	dut.clk = 1;
	dut.eval();

	itrace(dut.pc_copy);
}

void cpu_cycle(uint32_t i) {
	print_step = (i < 17);
	uint32_t j;
	for(j = 0; j <= i; j++) {
		CPU_STATUS = RUNNING;
		single_step();
#ifdef DIFFTEST_ON
		difftest_step();
#endif
		if(CPU_STATUS == EBREAK) {
			if(dut.reg_copy[10] == 0) {
				printf("HIT GOOD TRAP\n");
				CPU_STATUS = END;
				break;
			} 
			else {
				printf("HIT BAD TRAP\n");
				CPU_STATUS = EXCEPT;
				break;
			}
		} 
		else if (CPU_STATUS == EXCEPT) {
			break;
		}
		//nvboard_update();
	}
	if (CPU_STATUS != END && CPU_STATUS != EXCEPT)
		CPU_STATUS = PAUSE;
}

void cpu_init() {
	dut.rst = 0;
	dut.eval();
	dut.rst = 1;
	dut.eval();
	dut.rst = 0;
	CPU_STATUS = START;
}
