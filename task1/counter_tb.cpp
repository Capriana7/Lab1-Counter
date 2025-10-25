#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char **argv, char **env){
    int i;
    int clk;

    Verilated::commandArgs(argc, argv);

    Vcounter* top = new Vcounter;

    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("counter.vcd");

    top->clk = 1;
    top->rst = 1;
    top->en = 0;
    int count = top->count;
    bool pause = false;
    int pause_cycles = 0;

    for(i=0; i<300;i++){
        for(clk=0; clk<2; clk++){
            tfp->dump(2*i+clk);
            top->clk = !top->clk;
            top->eval ();
        }
        top->rst = (i<2) | (i==20) | (i==50); // random resets added
        if (top->clk) {
            int count = top->count;
            if (!pause && count == 9) { // pause implemented on first cycle of count hitting 9
                pause = true;
                pause_cycles = 0;
            }
            if (pause) {
                top->en = 0;
                pause_cycles++;
                if (pause_cycles > 3){ // waits for 3 whole clock cycles of enable being low
                    pause = false;
                    top->en = 1;
                }
            } 
            else if (i<100){ // if it is not paused or 9, it will keep counting until 100 clock cycles
                top->en = 1;
            }
        }
        if (Verilated::gotFinish()) exit(0);
    }
    tfp->close();
    exit(0);
}

