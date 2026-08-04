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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	char expr[1024];
	uint32_t val;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

int new_wp(char *e) {
	if (!free_) {
		printf("no free watchpoint!\n");
		return -1;
	}
	uint32_t v;
	bool success;
	v = expr(e, &success);
	if (!success) {
		printf("wrong expression\n");
		return -1;
	}
	WP *node = free_;
	free_ = node -> next;
	node -> next = head;
	head = node;
	strncpy(node -> expr, e, 1024);
	node -> val = v;
	return 0;
}

int free_wp(int id) {
	WP *node = head;
	if (head == NULL) {
		printf("no wp in use\n");
		return -1;
	}
	if (id < 0 || id >= NR_WP) {
		printf("invalid id!\n");
		return -1;
	}
	if (head -> NO == id) {
		head = node -> next;
		node -> next = free_;
		free_ = node;
		return 0;
	}
	int found = 0;
	while (node->next != NULL) {
		if (node -> next -> NO == id) {
			found = 1;
			break;
		}
	}
	if(found == 0) {
		printf("id not found\n");
		return -1;
	}
	node -> next = wp_pool[id].next;
	wp_pool[id].next = free_;
	free_ = &wp_pool[id];
	return 0;
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void check_wp() {
	bool success, changed;
	changed = false;
	uint32_t v;
	WP *node = head;
	while (node != NULL) {
		v = expr(node -> expr, &success);
		if (!success){
			printf("watchpoint id:%d\n", node -> NO);
			printf("wrong expression: %s\n", node -> expr);
			changed = true;
		}
		if (v != node -> val){
			printf("watchpoint id:%d\n", node -> NO);
			printf("expression changed: %s\n", node -> expr);
			printf("former value:%d, changed value:%d\n", node -> val, v);
			node -> val = v;
			changed = true;
		}
		node = node -> next;
	}
	if (changed && nemu_state.state == NEMU_RUNNING) nemu_state.state = NEMU_STOP;
}

void print_watchpoint() {
	if (head == NULL) {
		printf("no watchpoint\n");
	}
	else {
		WP *node = head;
		while (node != NULL) {
			printf("id:%2d value:%10d expression:%s\n", node -> NO, node -> val, node -> expr);
			node = node -> next;
		}
	}
}
