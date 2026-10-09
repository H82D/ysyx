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

#include <common.h>

extern uint64_t g_nr_guest_inst;

#ifndef CONFIG_TARGET_AM
FILE *log_fp = NULL;
FILE *iringbuf_fp = NULL;
FILE *mtrace_fp = NULL;
FILE *ftrace_fp = NULL;

void init_log(const char *log_file) {
	char iringbuf_file[256];
	char mtrace_file[256];
	char ftrace_file[256];
	strncpy(iringbuf_file, log_file, 255);
	strncpy(mtrace_file, log_file, 255);
	strncpy(ftrace_file, log_file, 255);
	int i;
	for (i = 0; log_file[i]; i++);
	strncpy(iringbuf_file + i - 8, "-iringbuf.txt", 14);
	strncpy(mtrace_file + i - 8, "-mtrace.txt", 12);
	strncpy(ftrace_file + i - 8, "-ftrace.txt", 12);
	
  log_fp = stdout;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;

		fp = fopen(iringbuf_file, "w");
    Assert(fp, "Can not open '%s'", iringbuf_file);
		iringbuf_fp = fp;

		fp = fopen(mtrace_file, "w");
    Assert(fp, "Can not open '%s'", mtrace_file);
		mtrace_fp = fp;

		fp = fopen(ftrace_file, "w");
    Assert(fp, "Can not open '%s'", ftrace_file);
		ftrace_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
}

bool log_enable() {
  return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
         (g_nr_guest_inst <= CONFIG_TRACE_END), false);
}
#endif
