#include "Vex_switch.h"
#include "verilated.h"
#include "verilated_fst_c.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

int main(int argc, char *argv[]){
		Verilated::traceEverOn(true);
		VerilatedFstC *tfp = new VerilatedFstC;

		VerilatedContext *contextp = new VerilatedContext;
		contextp->traceEverOn(true);
		contextp -> commandArgs(argc, argv);
		Vex_switch *top = new Vex_switch{contextp};
		top->trace(tfp, 99);
		tfp->open("./waveform/ex_switch.fst");

		srand(time(NULL));
		int i=0;
		int clk=0;
		while(i<100){
				clk++;
				int a = rand() % 2;
				int b = rand() % 2;
				top->a = a;
				top->b = b;
				top->eval();
				tfp->dump(clk);
				//printf("a=%d, b=%d, f=%d\n", a, b, top->f);
				i++;
		}
		tfp->close();
		delete top;
		delete contextp;
		return 0;
}
