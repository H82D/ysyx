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
#include <memory/vaddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#define MAX_TOKEN 32768

enum {
	 TK_NOTYPE = 256, TK_EQ, TK_NUM_DEC, TK_NEQ, TK_NUM_HEX, TK_REG, TK_AND, TK_NEG, TK_PTR,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"\\+", '+'},         // plus
	{"-", '-'}, 					// minus
	{"\\*", '*'}, 				// times
	{"/", '/'}, 					// divide
	{"\\(", '('},					// left bracket
	{"\\)", ')'}, 				// right bracket
  {" +", TK_NOTYPE},    // spaces
	{"==", TK_EQ},        // equal
	{"!=", TK_NEQ}, 			// not equal
	{"&&", TK_AND}, 			// and
	{"\\$[a-zA-Z0-9]+", TK_REG},	// register
	{"0x[a-fA-F0-9]{1,31}", TK_NUM_HEX},	// hex number
	{"[0-9]{1,31}", TK_NUM_DEC},	// dec number
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[MAX_TOKEN] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //   i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

				if (nr_token >= MAX_TOKEN){
					printf("expression too long\n");
					return false;
				}

        switch (rules[i].token_type) {
					case TK_NOTYPE:
						break;
					case '+':
						tokens[nr_token].type = '+';
						break;
					case '-':
						if(nr_token == 0
								|| tokens[nr_token - 1].type == TK_NUM_HEX
								|| tokens[nr_token - 1].type == TK_NUM_DEC
								|| tokens[nr_token - 1].type == TK_REG
								|| tokens[nr_token - 1].type == ')')
							tokens[nr_token].type = '-';
						else tokens[nr_token].type = TK_NEG;
						break;
					case '*':
						if(nr_token == 0
								|| tokens[nr_token - 1].type == TK_NUM_HEX
								|| tokens[nr_token - 1].type == TK_NUM_DEC
								|| tokens[nr_token - 1].type == TK_REG
								|| tokens[nr_token - 1].type == ')')
							tokens[nr_token].type = '*';
						else tokens[nr_token].type = TK_PTR;
						break;
					case'/':
						tokens[nr_token].type = '/';
						break;
					case '(':
						tokens[nr_token].type = '(';
						break;
					case ')':
						tokens[nr_token].type = ')';
						break;
					case TK_EQ:
						tokens[nr_token].type = TK_EQ;
						break;
					case TK_NEQ:
						tokens[nr_token].type = TK_NEQ;
						break;
					case TK_NUM_DEC:
						tokens[nr_token].type = TK_NUM_DEC;
						break;
					case TK_NUM_HEX:
						tokens[nr_token].type = TK_NUM_HEX;
						break;
					case TK_REG:
						tokens[nr_token].type = TK_REG;
						break;
					case TK_AND:
						tokens[nr_token].type = TK_AND;
						break;
          default: 
						printf("wrong token type %d\n", rules[i].token_type);
						return false;
        }
				if(rules[i].token_type != TK_NOTYPE) {
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = '\0';
					nr_token++;
				}
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q){
	if (tokens[p].type != '(') return false;
	int pair = 0;
	for (int i = p; i <= q; i++){
		if (tokens[i].type == '(') pair++;
		if (tokens[i].type == ')') pair--;
		if (pair <= 0 && i != q) {
			return false;
		}
	}
	return (pair == 0);
}

static int find_main_oprator(int p, int q){
	int priority[MAX_TOKEN];
	int base = 0;
	int max = p;
	for (int i = p; i <= q; i++){
		switch(tokens[i].type){
			case '(':
				base -= 20;
				priority[i] = base + 1;
				break;
			case ')':
				base += 20;
				priority[i] = base + 1;
				break;
			case TK_PTR:
			case TK_NEG:
				priority[i] = base + 2;
				break;
			case '*':
			case '/':
				priority[i] = base + 3;
				break;
			case '+':
			case '-':
				priority[i] = base + 4;
				break;
			case TK_EQ:
			case TK_NEQ:
				priority[i] = base + 8;
				break;
			case TK_AND:
				priority[i] = base + 12;
				break;
			case TK_NUM_DEC:
			case TK_NUM_HEX:
			case TK_REG:
				priority[i] = base;
				break;
		}
		if (priority[i] >= priority[max]) max = i;
	}
	return max;
}
	

static uint32_t eval(int p, int q, bool *success) {
	if(p < 0 || q < 0) {
		//printf("invalid parameter:%d %d\n", p, q);
		*success = false;
		return 0;
	}
  if (p > q) {
  /* Bad expression */
		// printf("invalid parameter:%d %d\n", p, q);
		*success = false;
		return 0;
  }
	else if (p == q) {
  /* Single token.
	* For now this token should be a number.
	* Return the value of the number.
	*/
		if(tokens[p].type == TK_NUM_DEC){
			return strtol(tokens[p].str, NULL, 10);
		} 
		else if(tokens[p].type == TK_NUM_HEX){
			return strtol(tokens[p].str + 2, NULL, 16);
		}
		else if(tokens[p].type == TK_REG){
			return isa_reg_str2val(tokens[p].str + 1, success);
		}
		else {
			printf("invalid expression at position %d\n", p);
			*success = false;
			return 0;
		}
  }
  else if (check_parentheses(p, q)) {
  /* The expression is surrounded by a matched pair of parentheses.
	 * If that is the case, just throw away the parentheses.
	 */
		return eval(p + 1, q - 1, success);
  }
	else {
		*success = true;
		int op = p, val1 = 0, val2 = 0;
	  op = find_main_oprator(p, q);
    if(tokens[op].type != TK_PTR
				&& tokens[op].type != TK_NEG) 
			val1 = eval(p, op - 1, success);
    val2 = eval(op + 1, q, success);

		switch (tokens[op].type) {
	    case '+': 
				return val1 + val2;
      case '-': 
				return val1 - val2; 
      case '*': 
				return val1 * val2;
      case '/': 
				if(val2 != 0) return val1 / val2;
				else {
					*success = false;
					printf("divide by zero\n");
					return 0;
				}
			case TK_EQ: 
				return val1 == val2;
			case TK_NEQ: 
				return val1 != val2;
			case TK_AND: 
				return val1 && val2;
			case TK_PTR: 
				return vaddr_read(val2, 4);
			case TK_NEG:
				return -val2;
      default:	
				printf("invalid token type at position %d\n", op);
				*success = false;
				return 0;
		}
	}
}

uint32_t expr(char *e, bool *success) {
	*success = true;
  if (!make_token(e)) {
    *success = false;
		printf("make_token fail\n");
    return 0;
  }
	
	int braket = 0;
	for (int i = 0; i < nr_token; i++){
		if(tokens[i].type == '(') braket++;
		if(tokens[i].type == ')') braket--;
		if(braket < 0){
			*success = false;
			printf("wrong braket\n");
			return 0;
		}
	}
	if(braket > 0){
		*success = false;
		printf("wrong braket\n");
		return 0;
	}

  /* TODO: Insert codes to evaluate the expression. */
	return eval(0, nr_token-1, success);
}
