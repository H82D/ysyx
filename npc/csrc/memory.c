#include "common.h"

uint8_t* ram;
uint32_t img_size;

void ram_init(const char *img_file) {
	ram = (uint8_t*)malloc(sizeof(uint8_t) * RAM_SIZE);
	assert(ram);

	FILE* fp = fopen(img_file, "rb");
	assert(fp);

	fseek(fp, 0, SEEK_END);
	img_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	int ret = fread(ram, img_size, 1, fp);
	assert(ret == 1);

	fclose(fp);
}

static int check_addr(uint8_t* addr) {
	if(addr < ram || addr >= ram + RAM_SIZE) {
		printf("ERROR: trying to access address 0x%08lx\n", addr - ram + RAM_BASE);
		CPU_STATUS = EXCEPT;
		return -1;
	}
	return 0;
}

extern "C" uint32_t inst_fetch(uint32_t pc) {
	uint8_t *host_addr = ram + pc - RAM_BASE;
	if (pc == 0) {
		printf("WARNING: pc = 0, return nop\n");
		return 0x13;
	}
	else if (check_addr(host_addr)) {
		return -1;
	}
	else {
		return *(uint32_t*)host_addr;
	}
}

extern "C" uint32_t ram_read(bool en, uint32_t addr) {
	if (!en) return 0;
	uint8_t* host_addr = ram + addr - RAM_BASE;
	if (check_addr(host_addr) == 0)
		return *(uint32_t*)host_addr;
	else 
		return -1;
}

extern "C" bool ram_write(bool en, uint32_t addr, uint32_t data, int len) {
	if (!en)
		return false;
	else {
		if (addr == 0xa00003f8) {
			putc(data, stdout);
			fflush(stdout);
			return true;
		}
		uint8_t* host_addr = ram + addr - RAM_BASE;
		check_addr(host_addr);
		switch(len) {
			case 1:
				*(uint8_t*)host_addr = data & 0x000000ff;
				break;
			case 2:
				*(uint16_t*)host_addr = data & 0x0000ffff;
				break;
			case 4:
				*(uint32_t*)host_addr = data & 0xffffffff;
				break;
			default :
				inv_inst();
		}
		return true;
	}
}
