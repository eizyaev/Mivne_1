/* 046267 Computer Architecture - Spring 2016 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"
SIM_coreState Core;


typedef struct pipestate
{
        SIM_cmd cmd;      /// The processed command in each pipe stage
        int32_t src1Val;  /// Actual value of src1 (considering forwarding mux, etc.)
        int32_t src2Val;  /// Actual value of src2 (considering forwarding mux, etc.)
} pipestate;

pipestate fetch_cur, fetch_next, dec_cur, dec_next, exe_cur, exe_next, mem_cur, mem_next, wb_cur, wb_next; 

uint32_t ticks; // the current clk tick

void pipestage_fetch(void);
void pipestage_dec(void);
void pipestage_exe(void);
void pipestage_mem(void);
void pipestage_wb(void);
void UpdateCoreState(void);


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
    for ( i = 0 ; i < SIM_REGFILE_SIZE ; i++){

        Core.regFile[i] = 0;
    }        
    fetch_cur.cmd = Core.pipeStageState[0].cmd;
    fetch_next.cmd = Core.pipeStageState[0].cmd;
    dec_cur.cmd = Core.pipeStageState[1].cmd;
    dec_next.cmd = Core.pipeStageState[1].cmd;
    exe_cur.cmd = Core.pipeStageState[2].cmd;
    exe_next.cmd = Core.pipeStageState[2].cmd;
    mem_cur.cmd = Core.pipeStageState[3].cmd;
    mem_next.cmd = Core.pipeStageState[3].cmd;
    wb_cur.cmd = Core.pipeStageState[4].cmd;
    wb_next.cmd = Core.pipeStageState[4].cmd;

    return 0;
}

void SIM_CoreClkTick(void)
{
pipestage_fetch();
pipestage_dec();
pipestage_exe();
pipestage_mem();
pipestage_wb();
UpdateCoreState();
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
    SIM_MemInstRead(Core.pc, &fetch_next.cmd);
    dec_next.cmd = fetch_cur.cmd;// TODO FORWARDING / BRANCH HAZARD
    Core.pc += 4;
    fetch_cur = fetch_next;
}

void pipestage_dec(void)
{
    exe_next.cmd = dec_cur.cmd;// TODO FORWARDING / BRANCH HAZARD
    dec_cur = dec_next;
}

void pipestage_exe(void)
{
    mem_next.cmd = exe_cur.cmd;// TODO FORWARDING / BRANCH HAZARD
    switch (exe_cur.cmd.opcode)
    {
    case 0: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        break;
    case 1:
    	break;
    case 2:
        break;
    case 3:
    	break;
    case 4:
        break;
    case 5:
        break;
    case 6:
    	break;
    case 7:
        break;
    }
    exe_cur = exe_next;
}

void pipestage_mem(void)
{
    wb_next.cmd = mem_cur.cmd;
    mem_cur = mem_next;
}

void pipestage_wb(void)
{
    wb_cur = wb_next;
}

void UpdateCoreState(void)
{// VERY GOOD, FORWARDING WILL BE IMPLEMENTED IN OTHER FUNCTIONS
    Core.pipeStageState[0].cmd = fetch_cur.cmd;
    Core.pipeStageState[1].cmd = dec_cur.cmd;
    Core.pipeStageState[2].cmd = exe_cur.cmd;
    Core.pipeStageState[3].cmd = mem_cur.cmd;
    Core.pipeStageState[4].cmd = wb_cur.cmd;

}

