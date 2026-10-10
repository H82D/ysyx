#include <stdio.h>
#include <assert.h>
#include <elf.h>

struct {
	char	name[100];
	int		ent;
} func[100];
int func_num = 0;

static void readbytes(void *dst, FILE *fp, int offset, int len) {
	assert(fp);
	fseek(fp, offset, SEEK_SET);
	assert(fread((uint8_t*)dst, len, 1, fp) == 1);
}

static void readstring(void *dst, FILE *fp, int offset) {
	assert(fp);
	fseek(fp, offset, SEEK_SET);
	assert(fgets((char*)dst, 100, fp));
}

void init_ftrace(const char *elf_file) {
	FILE *fp = fopen(elf_file, "rb");
	assert(fp);

	Elf32_Ehdr ehdr;
	readbytes(&ehdr, fp, 0, sizeof(Elf32_Ehdr));
	assert(ehdr.e_ident[EI_MAG0] == ELFMAG0 &&
		ehdr.e_ident[EI_MAG1] == ELFMAG1 &&
		ehdr.e_ident[EI_MAG2] == ELFMAG2 &&
		ehdr.e_ident[EI_MAG3] == ELFMAG3);

	Elf32_Shdr shdr[100];
	readbytes(&shdr, fp, ehdr.e_shoff, ehdr.e_shentsize * ehdr.e_shnum);

	int strtab_idx = 0;
	int symtab_idx = 0;
	for (int i = 0; i < ehdr.e_shnum; i++) {
		if (shdr[i].sh_type == SHT_STRTAB && i != ehdr.e_shstrndx) 
			strtab_idx = i;
		if (shdr[i].sh_type == SHT_SYMTAB)
			symtab_idx = i;
	}

	Elf32_Sym sym[100];
	readbytes(sym, fp, shdr[symtab_idx].sh_offset, shdr[symtab_idx].sh_size);

	for (int i = 0; i < (shdr[symtab_idx].sh_size / sizeof(Elf32_Sym)); i++) {
		if ((sym[i].st_info & 0xf) == STT_FUNC) {
			readstring(func[func_num].name, fp, shdr[strtab_idx].sh_offset + sym[i].st_name);
			func[func_num].ent = sym[i].st_value;
			func_num++;
		}
	}

}
