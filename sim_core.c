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

typedef struct pipe_mem
{
        pipestate pipe;
        int32_t alu_result;
} pipe_mem;

typedef struct pipe_wb
{
        pipestate pipe;
        int32_t mem_load;
} pipe_wb;

pipestate fetch_cur, fetch_next, dec_cur, dec_next, exe_cur, exe_next; 
pipe_mem mem_cur, mem_next; // TODO: change reset, update functions
pipe_wb wb_cur, wb_next; // TODO: change reset, update functions


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
    /* at clk = 0 do fetch*/
    SIM_MemInstRead(Core.pc, &Core.pipeStageState[0].cmd);
    Core.pipeStageState[0].src1Val = 0;
    Core.pipeStageState[0].src2Val = 0;

    for ( i = 1 ; i < SIM_PIPELINE_DEPTH ; i++)
    {
        Core.pipeStageState[i].cmd.opcode = 0; 
        Core.pipeStageState[i].cmd.src1 = 0; 
        Core.pipeStageState[i].cmd.src2 = 0;
        Core.pipeStageState[i].cmd.isSrc2Imm = false; 
        Core.pipeStageState[i].cmd.dst = 0;
        Core.pipeStageState[i].src1Val = 0;
        Core.pipeStageState[i].src2Val = 0;
    }

        Core.regFile[i] = 0;
     
        fetch_cur.cmd = Core.pipeStageState[0].cmd;
        fetch_next.cmd = Core.pipeStageState[0].cmd;
        dec_cur.cmd = Core.pipeStageState[1].cmd;
        dec_next.cmd = Core.pipeStageState[1].cmd;
        exe_cur.cmd = Core.pipeStageState[2].cmd;
        exe_next.cmd = Core.pipeStageState[2].cmd;
        mem_cur.pipe.cmd = Core.pipeStageState[3].cmd;
        mem_next.pipe.cmd = Core.pipeStageState[3].cmd;
        wb_cur.pipe.cmd = Core.pipeStageState[4].cmd;
        wb_next.pipe.cmd = Core.pipeStageState[4].cmd;

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
    Core.pc += 4;
    SIM_MemInstRead(Core.pc, &fetch_next.cmd);
    dec_next.cmd = fetch_cur.cmd;// TODO FORWARDING / BRANCH HAZARD
    switch (fetch_cur.cmd.opcode)
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
    fetch_cur = fetch_next;
}

void pipestage_dec(void)
{
    exe_next.cmd = dec_cur.cmd;// TODO FORWARDING / BRANCH HAZARD
    exe_next.src1Val = Core.regFile[dec_cur.cmd.src1]; 
    exe_next.src2Val = Core.regFile[dec_cur.cmd.src2];
    switch (dec_cur.cmd.opcode)
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
    dec_cur = dec_next;
}

void pipestage_exe(void)
{
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
    mem_next.pipe = exe_cur; // TODO FORWARDING / BRANCH HAZARD
    mem_next.alu_result = mem_cur.pipe.src1Val + mem_cur.pipe.src2Val;
    exe_cur = exe_next;
}

void pipestage_mem(void)
{
    wb_next.pipe.cmd = mem_cur.pipe.cmd;
    
    if (mem_cur.pipe.cmd.opcode == 3) 
        if(SIM_MemDataRead(mem_cur.alu_result, &wb_next.mem_load) == -1)
            printf("\n############# stalled on load #############\n");
        else
            printf("\n############# didnt stalled ###############\n");
    switch (mem_cur.pipe.cmd.opcode)
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
    mem_cur = mem_next;
}

void pipestage_wb(void)
{
    Core.regFile[wb_cur.pipe.cmd.dst];
    wb_cur = wb_next;
}


void UpdateCoreState(void)
{
    Core.pipeStageState[0].cmd = fetch_cur.cmd;
    Core.pipeStageState[1].cmd = dec_cur.cmd;
    Core.pipeStageState[2].cmd = exe_cur.cmd;
    Core.pipeStageState[3].cmd = mem_cur.pipe.cmd;
    Core.pipeStageState[4].cmd = wb_cur.pipe.cmd;

}


