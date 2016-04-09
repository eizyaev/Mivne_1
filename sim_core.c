/* 046267 Computer Architecture - Spring 2016 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"
SIM_coreState Core;
uint32_t ticks; // the current clk tick

void pipestage_fetch(void);
void pipestage_dec(void);
void pipestage_exe(void);
void pipestage_mem(void);
void pipestage_wb(void);



int SIM_CoreReset(void)
{
    int i;
    Core.pc = 0;

    for ( i = 0 ; i < SIM_PIPELINE_DEPTH ; i++)
    {
        Core.pipeStageState[i].cmd.opcode = 0; 
        Core.pipeStageState[i].cmd.src1 = 0; 
        Core.pipeStageState[i].cmd.src2 = 0;
        Core.pipeStageState[i].cmd.isSrc2Imm = false; 
        Core.pipeStageState[i].cmd.dst = 0;
        Core.pipeStageState[i].src1Val = 0;
        Core.pipeStageState[i].src2Val = 0;
    }
    for ( i = 0 ; i < SIM_REGFILE_SIZE ; i++)
        Core.regFile[i] = 0;
    

    return 0;
}

void SIM_CoreClkTick(void)
{
pipestage_fetch();
pipestage_dec();
pipestage_exe();
pipestage_mem();
pipestage_wb();

++ticks;
}

void SIM_CoreGetState(SIM_coreState *curState)
{    
	curState->pc = Core.pc;
	for (int i=0;i<SIM_REGFILE_SIZE; ++i){
		curState->regFile[i]=Core.regFile[i];
	}
	for (int i=0;i<SIM_PIPELINE_DEPTH;++i){
		curState->pipeStageState[i].cmd.opcode = Core.pipeStageState[i].cmd.opcode;
		curState->pipeStageState[i].cmd.src1 = Core.pipeStageState[i].cmd.src1;
		curState->pipeStageState[i].cmd.src2 = Core.pipeStageState[i].cmd.src2;
		curState->pipeStageState[i].cmd.isSrc2Imm = Core.pipeStageState[i].cmd.isSrc2Imm;
		curState->pipeStageState[i].cmd.dst = Core.pipeStageState[i].cmd.dst;
		curState->pipeStageState[i].src1Val = Core.pipeStageState[i].src1Val;
		curState->pipeStageState[i].src2Val = Core.pipeStageState[i].src2Val;
	}

}


void pipestage_fetch(void)
{
}

void pipestage_dec(void)
{
}

void pipestage_exe(void)
{
}

void pipestage_mem(void)
{
}

void pipestage_wb(void)
{
}

