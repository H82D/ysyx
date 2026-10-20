#***************************************************************************************
# Copyright (c) 2014-2024 Zihao Yu, Nanjing University
#
# NEMU is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
#
# See the Mulan PSL v2 for more details.
#**************************************************************************************/

ifeq ($(CONFIG_ITRACE)$(CONFIG_IQUEUE),)
SRCS-BLACKLIST-y += $(NPC_HOME)/csrc/disasm.c
else
LIBCAPSTONE = $(NPC_HOME)/tools/capstone/repo/libcapstone.so.5
CXXFLAGS += -I $(NPC_HOME)/tools/capstone/repo/include

csrc/disasm.c: $(LIBCAPSTONE)
	@echo 8888888888888888888888888888888

$(LIBCAPSTONE):
	@echo 77777777777777777777777777777
	$(MAKE) -C tools/capstone
endif
