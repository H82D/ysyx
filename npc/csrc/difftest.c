#include "common.h"
#include <dlfcn.h>

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

void (*ref_difftest_memcpy)(uint32_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(uint32_t *pc, uint32_t *reg_array, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;

void difftest_init(const char *ref_so_file) {
#ifdef DIFFTEST_ON
	assert(ref_so_file);

	void *handle;
	handle = dlopen(ref_so_file, RTLD_LAZY);
	assert(handle);

	ref_difftest_memcpy = (void(*)(uint32_t, void*, size_t, bool))dlsym(handle, "difftest_memcpy");
	assert(ref_difftest_memcpy);

	ref_difftest_regcpy = (void(*)(uint32_t*, uint32_t*, bool))dlsym(handle, "difftest_regcpy");
	assert(ref_difftest_regcpy);

	ref_difftest_exec = (void(*)(uint64_t))dlsym(handle, "difftest_exec");
	assert(ref_difftest_exec);

	void (*ref_difftest_init)(int) = (void(*)(int))dlsym(handle, "difftest_init");
	assert(ref_difftest_init);

	ref_difftest_init(0);
	ref_difftest_memcpy(RAM_BASE, ram, img_size, DIFFTEST_TO_REF);
	uint32_t dut_reg_copy[16];
	for (int i = 0; i < 16; i++)
		dut_reg_copy[i] = dut.reg_copy[i];
	ref_difftest_regcpy(&dut.pc_copy, dut_reg_copy, DIFFTEST_TO_REF);
#endif
}
static void report() {
	uint32_t ref_pc;
	uint32_t ref_gpr[16];
	ref_difftest_regcpy(&ref_pc, ref_gpr, DIFFTEST_TO_DUT);
	printf("disagreement at pc = 0x%08x\n", dut.pc_copy);
	printf("registers :\n");
	for(int i = 0; i < 16; i++) {
		if (dut.reg_copy[i] == ref_gpr[i])
			printf("x%-2d = 0x%08x\n", i, dut.reg_copy[i]);
		else
			printf("dut-x%-2d = 0x%08x, ref-x%-2d = 0x%08x\n", i, dut.reg_copy[i], i, ref_gpr[i]);
	}
}


static void checkregs() {
	uint32_t ref_pc;
	uint32_t ref_gpr[16];
	ref_difftest_regcpy(&ref_pc, ref_gpr, DIFFTEST_TO_DUT);
	/*
	if (dut.pc_copy != ref_pc) {
		CPU_STATUS = EXCEPT;
		report();
	}
	*/
	for (int i = 0; i < 16; i++) {
		if (dut.reg_copy[i] != ref_gpr[i]) {
			CPU_STATUS = EXCEPT;
			report();
		}
	}
}

void difftest_step() {
	ref_difftest_exec(1);
	checkregs();
}
