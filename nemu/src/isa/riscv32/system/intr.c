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

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */

	// MIE = mstatus[3], MPIE = mstatus[7]
	int mie = (cpu.csr[mstatus] >> 3) & 1;
	if (mie == 0)
		cpu.csr[mstatus] = cpu.csr[mstatus] & (0xffffffff - (1 << 7));
	else
		cpu.csr[mstatus] = cpu.csr[mstatus] | (1 << 7);
	cpu.csr[mstatus] = cpu.csr[mstatus] & (0xffffffff - (1 << 3));
	
	cpu.csr[mcause] = NO;
	cpu.csr[mepc] = epc;
  return cpu.csr[mtvec];
}

word_t isa_finish_intr() {
	int mpie = (cpu.csr[mstatus] >> 7) & 1;
	if (mpie == 0)
		cpu.csr[mstatus] = cpu.csr[mstatus] & (0xffffffff - (1 << 3));
	else
		cpu.csr[mstatus] = cpu.csr[mstatus] | (1 << 3);
	cpu.csr[mstatus] = cpu.csr[mstatus] | (1 << 7);
	return cpu.csr[mepc];
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
