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
        int32_t alu_mem_result;
} pipe_wb;

pipestate fetch_cur, fetch_next, dec_cur, dec_next, exe_cur, exe_next; 
pipe_mem mem_cur, mem_next; // TODO: change reset, update functions
pipe_wb wb_cur, wb_next; // TODO: change reset, update functions
bool stalled = false;
int st_cnt = 0;

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
    for(i=0;i<SIM_REGFILE_SIZE;++i){
        Core.regFile[i] = 0;
    	}
    	//Core.regFile[9] = 16;
    	//Core.regFile[1]=13;
		//Core.regFile[12]=11;
		//Core.regFile[4]=33;
		//Core.regFile[5]=20;
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

    if (stalled)
        Core.pc -= 4;

    UpdateCoreState();

}

void SIM_CoreGetState(SIM_coreState *curState)
{    
	curState->pc = Core.pc;
	int i;
	for (i=0;i<SIM_REGFILE_SIZE; ++i){
		curState->regFile[i]=Core.regFile[i];
	}
	for (i=0;i<SIM_PIPELINE_DEPTH;++i){
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
    case CMD_NOP: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        dec_next.src1Val = 0;
        dec_next.src2Val = 0;
        break;
    case CMD_ADD:
        dec_next.src1Val = Core.regFile[fetch_cur.cmd.src1];
        dec_next.src2Val = Core.regFile[fetch_cur.cmd.src2];
    	break;
    case CMD_SUB:
        dec_next.src1Val = Core.regFile[fetch_cur.cmd.src1];
        dec_next.src2Val = Core.regFile[fetch_cur.cmd.src2];
        break;
    case CMD_LOAD:
        dec_next.src1Val = Core.regFile[fetch_cur.cmd.src1];
        dec_next.src2Val = fetch_cur.cmd.src2;
    	break;
    case CMD_STORE:
        dec_next.src1Val = Core.regFile[fetch_cur.cmd.src1];
        dec_next.src2Val = fetch_cur.cmd.src2;
        break;
    case CMD_BR:
        break;
    case CMD_BREQ:
    	break;
    case CMD_BRNEQ:
        break;
    }
}

void pipestage_dec(void)
{
    exe_next = dec_cur;// TODO FORWARDING / BRANCH HAZARD
    switch (dec_cur.cmd.opcode)
    {
    case 0: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        exe_next.src1Val = 0;
        exe_next.src2Val = 0;
        break;
    case 1:
    	break;
    case 2:
        break;
    case CMD_LOAD:
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

}       

void pipestage_exe(void)
{

    mem_next.pipe = exe_cur; // TODO FORWARDING / BRANCH HAZARD
    switch (exe_cur.cmd.opcode)
    {
    case 0: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        mem_next.pipe.src1Val = 0;
        mem_next.pipe.src2Val = 0;
        mem_next.pipe.cmd.src1 = 0;
        mem_next.pipe.cmd.src2 = 0;
        mem_next.pipe.cmd.isSrc2Imm = false;
        mem_next.pipe.cmd.dst = 0;
        break;
    case 1:
        mem_next.pipe = exe_cur; // TODO FORWARDING / BRANCH HAZARD
        mem_next.alu_result = exe_cur.src1Val + exe_cur.src2Val;
        mem_next.pipe.src1Val = exe_cur.src1Val;
        mem_next.pipe.src2Val = exe_cur.src2Val;
    	break;
    case 2:
        mem_next.pipe = exe_cur; // TODO FORWARDING / BRANCH HAZARD
        mem_next.alu_result = exe_cur.src1Val - exe_cur.src2Val;
        mem_next.pipe.src1Val = exe_cur.src1Val;
        mem_next.pipe.src2Val = exe_cur.src2Val;
        break;
    case 3:
            mem_next.alu_result = exe_cur.src1Val + exe_cur.src2Val;
    	break;
    case 4:
            mem_next.alu_result = exe_cur.src1Val + exe_cur.src2Val;
        break;
    case 5:
        break;
    case 6:
    	break;
    case 7:
        break;
    }
}

void pipestage_mem(void)
{
    wb_next.pipe.cmd = mem_cur.pipe.cmd;
    switch (mem_cur.pipe.cmd.opcode)
    {
    case 0: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        wb_next.pipe.src1Val = 0;
        wb_next.pipe.src2Val = 0;
        break;
    case 1:
    	wb_next.alu_mem_result=mem_cur.alu_result;
        wb_next.pipe.src1Val = mem_cur.pipe.src1Val;
        wb_next.pipe.src2Val = mem_cur.pipe.src2Val;
        Core.regFile[wb_next.pipe.cmd.dst] =mem_cur.alu_result;
        //printf(" dec_cur %d  dst %d",dec_next.cmd.src1,wb_next.pipe.cmd.dst);
    	if (dec_next.cmd.src1==wb_next.pipe.cmd.dst)
    		dec_next.src1Val = mem_cur.alu_result;
    	if(dec_next.cmd.src2==wb_next.pipe.cmd.dst)//src2 might be immediate equaling to register index
    		dec_next.src2Val =mem_cur.alu_result;
    	break;
    case 2:
    	wb_next.alu_mem_result=mem_cur.alu_result;
        wb_next.pipe.src1Val = mem_cur.pipe.src1Val;
        wb_next.pipe.src2Val = mem_cur.pipe.src2Val;
    	Core.regFile[wb_next.pipe.cmd.dst] =mem_cur.alu_result;
    	if (dec_next.cmd.src1==wb_next.pipe.cmd.dst)
    		dec_next.src1Val = mem_cur.alu_result;
    	if(dec_next.cmd.src2==wb_next.pipe.cmd.dst)//src2 might be immediate equaling to register index
    		dec_next.src2Val =mem_cur.alu_result;
        break;
    case 3:
        if(SIM_MemDataRead((uint32_t)mem_cur.alu_result, &wb_next.mem_load) == -1)
        {
            st_cnt++;
            stalled = true;
            printf("\n######################stalllllleddd!@#!@#!@ : #######################\n");
        }
        else
        {
            stalled = false;
            st_cnt = 0;
            Core.regFile[wb_next.pipe.cmd.dst] = wb_next.mem_load;
    	    if (exe_next.cmd.src1 == wb_next.pipe.cmd.dst)
    		    exe_next.src1Val = wb_next.mem_load;
    	    if((exe_next.cmd.src2 == wb_next.pipe.cmd.dst) && (exe_next.cmd.isSrc2Imm == false))
    		    exe_next.src2Val = wb_next.mem_load;
        }
        wb_next.pipe.src1Val = mem_cur.pipe.src1Val;
        wb_next.pipe.src2Val = mem_cur.pipe.src2Val;
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
        //printf("\n###################### adress : %x  #######################\n", mem_cur.alu_result);
      //  printf("\n###################### mem_load adr : %d  #######################\n", &wb_next.mem_load);
    //    printf("\n###################### loaded Value : %x  #######################\n", wb_next.mem_load);
    if (((exe_cur.cmd.dst == dec_cur.cmd.src1) || (exe_cur.cmd.dst == dec_cur.cmd.src2 && dec_cur.cmd.isSrc2Imm == false)) 
          && (exe_cur.cmd.opcode == CMD_LOAD) && (dec_cur.cmd.opcode != CMD_NOP) && !stalled )
    {
        printf("\n###################### Data Hazard Detected!! : #######################\n");
        Core.pc -= 4;
        exe_next.cmd.opcode = CMD_NOP;
        exe_next.cmd.src1 = 0;
        exe_next.cmd.src2 = 0;
        exe_next.cmd.isSrc2Imm = false;
        exe_next.cmd.dst = 0;
        exe_next.src1Val = 0;
        exe_next.src2Val = 0;
    }
    else if(!stalled)
    {
        fetch_cur = fetch_next;
        dec_cur = dec_next;
    }
    if (!stalled)
    {
        exe_cur = exe_next;
        mem_cur = mem_next;
    }


}

void pipestage_wb(void)
{
    switch (wb_cur.pipe.cmd.opcode)
    
    {
    case 0: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        break;
    case 1:
    	Core.regFile[wb_cur.pipe.cmd.dst]=wb_cur.alu_mem_result;
    	if (exe_cur.cmd.src1==wb_cur.pipe.cmd.dst)
    		exe_cur.src1Val = wb_cur.alu_mem_result;
    	if(exe_cur.cmd.src2==wb_cur.pipe.cmd.dst)//src2 might be immediate equaling to register index
    		exe_cur.src2Val =wb_cur.alu_mem_result;
    	break;
    case 2:
    	if (exe_cur.cmd.src1==wb_cur.pipe.cmd.dst)//src2 might be immediate equaling to register index
    		exe_cur.src1Val = wb_cur.alu_mem_result;
    	if(exe_cur.cmd.src2==wb_cur.pipe.cmd.dst)
    		exe_cur.src2Val =wb_cur.alu_mem_result;
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
 
    wb_cur = wb_next;
    if (stalled == true)
    {
        wb_cur.pipe.cmd.opcode = CMD_NOP;
        wb_cur.pipe.cmd.src1 = 0; 
        wb_cur.pipe.cmd.src2 = 0;
        wb_cur.pipe.cmd.isSrc2Imm = false; 
        wb_cur.pipe.cmd.dst = 0;
        wb_cur.pipe.src1Val = 0;
        wb_cur.pipe.src2Val = 0;
    }
}


void UpdateCoreState(void)
{
    Core.pipeStageState[0].cmd = fetch_cur.cmd;
    Core.pipeStageState[0].src1Val = fetch_cur.src1Val;
    Core.pipeStageState[0].src2Val = fetch_cur.src2Val;
    Core.pipeStageState[1].cmd = dec_cur.cmd;
    Core.pipeStageState[1].src1Val = dec_cur.src1Val;
    Core.pipeStageState[1].src2Val = dec_cur.src2Val;
    Core.pipeStageState[2].cmd = exe_cur.cmd;
    Core.pipeStageState[2].src1Val = exe_cur.src1Val;
    Core.pipeStageState[2].src2Val = exe_cur.src2Val;
    Core.pipeStageState[3].cmd = mem_cur.pipe.cmd;
    Core.pipeStageState[3].src1Val = mem_cur.pipe.src1Val;
    Core.pipeStageState[3].src2Val = mem_cur.pipe.src2Val;
    Core.pipeStageState[4].cmd = wb_cur.pipe.cmd;
    Core.pipeStageState[4].src1Val = wb_cur.pipe.src1Val;
    Core.pipeStageState[4].src2Val = wb_cur.pipe.src2Val;

}


