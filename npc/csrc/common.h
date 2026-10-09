#include <Vnpc.h>
#include <nvboard.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DIFFTEST_ON

#define RAM_BASE 0x80000000
#define RAM_SIZE 0x8000000				// from 0x80000000 to 0x88000000
extern uint8_t *ram;
extern uint32_t img_size;
void ram_init(const char *img_file);
extern "C" uint32_t ram_read(uint32_t addr);
extern "C" bool ram_write(bool en, uint32_t addr, uint32_t data, int len);

extern TOP_NAME dut;
extern FILE *itrace_fp;
extern "C" int inv_inst();
extern "C" int ebreak();

enum CPU_status {
	START, RUNNING, PAUSE, END, EBREAK, EXCEPT,
};
extern enum CPU_status CPU_STATUS;
void cpu_cycle(uint32_t i);
void cpu_init();

void difftest_init(const char *ref_so_file);
void difftest_step();

void monitor_init();
