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
bool branched = false;

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
    //	Core.regFile[16]=-8;
    	//Core.regFile[1]=1;
	//	Core.regFile[11]=11;
		//Core.regFile[29]=33;

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
    branched = false;
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
        if (fetch_cur.cmd.isSrc2Imm)
            dec_next.src2Val = fetch_cur.cmd.src2;
        else
            dec_next.src2Val = Core.regFile[fetch_cur.cmd.src2];
        break;
    case CMD_STORE:
        dec_next.src1Val = Core.regFile[fetch_cur.cmd.src1];
        if (fetch_cur.cmd.isSrc2Imm)
            dec_next.src2Val = fetch_cur.cmd.src2;
        else
            dec_next.src2Val = Core.regFile[fetch_cur.cmd.src2];
        break;
    case CMD_BR:
        dec_next.src1Val = 0;
        dec_next.src2Val = 0;
        break;
    case 6:
    	dec_next.src1Val=Core.regFile[fetch_cur.cmd.src1];
    	dec_next.src2Val=Core.regFile[fetch_cur.cmd.src2];
    	break;

    case 7:
    	dec_next.src1Val=Core.regFile[fetch_cur.cmd.src1];
    	dec_next.src2Val=Core.regFile[fetch_cur.cmd.src2];
        break;
    }
}

void pipestage_dec(void)
{
    exe_next = dec_cur;// TODO FORWARDING / BRANCH HAZARD
    switch (dec_cur.cmd.opcode)
    {
    case 0: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        break;
    case 1:
    	break;
    case 2:
        break;
    case CMD_LOAD:
    	break;
    case CMD_STORE:
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
        mem_next.alu_result = exe_cur.src1Val + exe_cur.src2Val;
    	if (exe_next.cmd.src1==exe_cur.cmd.dst)
    		exe_next.src1Val = mem_next.alu_result;
    	if(exe_next.cmd.src2==exe_cur.cmd.dst && exe_next.cmd.isSrc2Imm==false)//src2 might be immediate equaling to register index
    		exe_next.src2Val =mem_next.alu_result;

    	break;
    case 2:
        mem_next.alu_result = exe_cur.src1Val - exe_cur.src2Val;
    	if (exe_next.cmd.src1==exe_cur.cmd.dst)
    		exe_next.src1Val = mem_next.alu_result;
    	if(exe_next.cmd.src2==exe_cur.cmd.dst && exe_next.cmd.isSrc2Imm==false)//src2 might be immediate equaling to register index
    		exe_next.src2Val =mem_next.alu_result;

        break;
    case 3:

            mem_next.alu_result = exe_cur.src1Val + exe_cur.src2Val;

    	break;
    case 4:
            mem_next.alu_result = Core.regFile[exe_cur.cmd.dst] + exe_cur.src2Val;
        break;
    case 5:
        mem_next.pipe.src1Val = 0;
        mem_next.pipe.src2Val = 0;
        break;
    case 6:
        mem_next.pipe = exe_cur; // TODO FORWARDING / BRANCH HAZARD
        mem_next.alu_result = exe_cur.src1Val - exe_cur.src2Val;
    	break;
    case 7:
        mem_next.pipe = exe_cur; // TODO FORWARDING / BRANCH HAZARD
        mem_next.alu_result = exe_cur.src1Val - exe_cur.src2Val;
        break;
    }
}

void pipestage_mem(void)
{
    wb_next.pipe = mem_cur.pipe;
    switch (mem_cur.pipe.cmd.opcode)
    {
    case 0: // TODO {"NOP", "ADD", "SUB", "LOAD", "STORE", "BR", "BREQ", "BRNEQ" }
        wb_next.pipe.src1Val = 0;
        wb_next.pipe.src2Val = 0;
        break;
    case 1:
    	wb_next.alu_mem_result=mem_cur.alu_result;
        Core.regFile[wb_next.pipe.cmd.dst] =mem_cur.alu_result;
    	if (dec_next.cmd.src1==mem_cur.pipe.cmd.dst)
    		dec_next.src1Val = mem_cur.alu_result;
    	if(dec_next.cmd.src2==mem_cur.pipe.cmd.dst && exe_next.cmd.isSrc2Imm==false)//src2 might be immediate equaling to register index
    		dec_next.src2Val =mem_cur.alu_result;
    	if (exe_next.cmd.src1==mem_cur.pipe.cmd.dst && ((exe_cur.cmd.dst !=exe_next.cmd.src1)||(exe_cur.cmd.opcode==CMD_LOAD||exe_cur.cmd.opcode==CMD_STORE)))
    		exe_next.src1Val = mem_cur.alu_result;
    	if(exe_next.cmd.src2==mem_cur.pipe.cmd.dst && exe_next.cmd.isSrc2Imm==false&& ((exe_cur.cmd.dst !=exe_next.cmd.src2)||(exe_cur.cmd.opcode==CMD_LOAD||exe_cur.cmd.opcode==CMD_STORE)))//src2 might be immediate equaling to register index
    		exe_next.src2Val =mem_cur.alu_result;

    	break;
    case 2:
    	wb_next.alu_mem_result=mem_cur.alu_result;
    	Core.regFile[wb_next.pipe.cmd.dst] =mem_cur.alu_result;
    	if (dec_next.cmd.src1==mem_cur.pipe.cmd.dst)
    		dec_next.src1Val = mem_cur.alu_result;
    	if(dec_next.cmd.src2==mem_cur.pipe.cmd.dst && dec_next.cmd.isSrc2Imm==false)//src2 might be immediate equaling to register index
    		dec_next.src2Val =mem_cur.alu_result;
    	if (exe_next.cmd.src1==mem_cur.pipe.cmd.dst  && ((exe_cur.cmd.dst !=exe_next.cmd.src1)||(exe_cur.cmd.opcode==CMD_LOAD||exe_cur.cmd.opcode==CMD_STORE)))
    		exe_next.src1Val = mem_cur.alu_result;
    	if(exe_next.cmd.src2==mem_cur.pipe.cmd.dst && exe_next.cmd.isSrc2Imm==false&&((exe_cur.cmd.dst !=exe_next.cmd.src2)||(exe_cur.cmd.opcode==CMD_LOAD||exe_cur.cmd.opcode==CMD_STORE)))//src2 might be immediate equaling to register index
    		exe_next.src2Val =mem_cur.alu_result;
        break;
    case CMD_LOAD:
        if(SIM_MemDataRead((uint32_t)mem_cur.alu_result, &wb_next.mem_load) == -1)
            stalled = true;
        else
        {
            stalled = false;
            Core.regFile[wb_next.pipe.cmd.dst] = wb_next.mem_load;
    	    if (exe_next.cmd.src1 == wb_next.pipe.cmd.dst)
            {
    		    exe_next.src1Val = wb_next.mem_load;
                   dec_cur.src1Val = wb_next.mem_load;
            }
    	    if((exe_next.cmd.src2 == wb_next.pipe.cmd.dst) && (exe_next.cmd.isSrc2Imm == false))
            {
    		    exe_next.src2Val = wb_next.mem_load;
                   dec_cur.src2Val = wb_next.mem_load;
            }
    	    if (dec_next.cmd.src1 == wb_next.pipe.cmd.dst)
            {
    		    dec_next.src1Val = wb_next.mem_load;
            }
    	    if((dec_next.cmd.src2 == wb_next.pipe.cmd.dst) && (dec_next.cmd.isSrc2Imm == false))
            {
    		    dec_next.src2Val = wb_next.mem_load;
            }



        }
    	break;
    case CMD_STORE:
            SIM_MemDataWrite((uint32_t)mem_cur.alu_result, mem_cur.pipe.src1Val);
        break;
    case 5:
        branched = true;
		Core.pc=Core.pc+ Core.regFile[mem_cur.pipe.cmd.dst]-12;
		SIM_MemInstRead(Core.pc, &fetch_next.cmd);
		fetch_next.src1Val=0;
		fetch_next.src2Val=0;
		dec_next.cmd.dst=0;
		dec_next.cmd.isSrc2Imm=false;
		dec_next.cmd.opcode=0;
		dec_next.cmd.src1=0;
		dec_next.cmd.src2=0;
		dec_next.src1Val=0;
		dec_next.src2Val=0;
		exe_next.cmd.dst=0;
		exe_next.cmd.isSrc2Imm=false;
		exe_next.cmd.opcode=0;
		exe_next.cmd.src1=0;
		exe_next.cmd.src2=0;
		exe_next.src1Val=0;
		exe_next.src2Val=0;
		mem_next.pipe.cmd.dst=0;
		mem_next.pipe.cmd.isSrc2Imm=false;
		mem_next.pipe.cmd.opcode=0;
		mem_next.pipe.cmd.src1=0;
		mem_next.pipe.cmd.src2=0;
		mem_next.pipe.src1Val=0;
		mem_next.pipe.src2Val=0;
    	break;
    case 6:
    	if (mem_cur.alu_result==0){
            branched = true;
			Core.pc=Core.pc+ Core.regFile[mem_cur.pipe.cmd.dst]-12;
			SIM_MemInstRead(Core.pc, &fetch_next.cmd);
			fetch_next.src1Val=0;
			fetch_next.src2Val=0;
			dec_next.cmd.dst=0;
			dec_next.cmd.isSrc2Imm=false;
			dec_next.cmd.opcode=0;
			dec_next.cmd.src1=0;
			dec_next.cmd.src2=0;
			dec_next.src1Val=0;
			dec_next.src2Val=0;
			exe_next.cmd.dst=0;
			exe_next.cmd.isSrc2Imm=false;
			exe_next.cmd.opcode=0;
			exe_next.cmd.src1=0;
			exe_next.cmd.src2=0;
			exe_next.src1Val=0;
			exe_next.src2Val=0;
			mem_next.pipe.cmd.dst=0;
			mem_next.pipe.cmd.isSrc2Imm=false;
			mem_next.pipe.cmd.opcode=0;
			mem_next.pipe.cmd.src1=0;
			mem_next.pipe.cmd.src2=0;
			mem_next.pipe.src1Val=0;
			mem_next.pipe.src2Val=0;
    	}
    	break;
    case 7:
    	if (mem_cur.alu_result!=0){
            branched = true;
			Core.pc=Core.pc+ Core.regFile[mem_cur.pipe.cmd.dst]-12;
			SIM_MemInstRead(Core.pc, &fetch_next.cmd);
			fetch_next.src1Val=0;
			fetch_next.src2Val=0;
			dec_next.cmd.dst=0;
			dec_next.cmd.isSrc2Imm=false;
			dec_next.cmd.opcode=0;
			dec_next.cmd.src1=0;
			dec_next.cmd.src2=0;
			dec_next.src1Val=0;
			dec_next.src2Val=0;
			exe_next.cmd.dst=0;
			exe_next.cmd.isSrc2Imm=false;
			exe_next.cmd.opcode=0;
			exe_next.cmd.src1=0;
			exe_next.cmd.src2=0;
			exe_next.src1Val=0;
			exe_next.src2Val=0;
			mem_next.pipe.cmd.dst=0;
			mem_next.pipe.cmd.isSrc2Imm=false;
			mem_next.pipe.cmd.opcode=0;
			mem_next.pipe.cmd.src1=0;
			mem_next.pipe.cmd.src2=0;
			mem_next.pipe.src1Val=0;
			mem_next.pipe.src2Val=0;
    	}
        break;
    }
    if (((exe_cur.cmd.dst == dec_cur.cmd.src1) || (exe_cur.cmd.dst == dec_cur.cmd.src2 && dec_cur.cmd.isSrc2Imm == false)
         || ((exe_cur.cmd.dst == dec_cur.cmd.dst) && dec_cur.cmd.opcode == CMD_STORE)) 
          && (exe_cur.cmd.opcode == CMD_LOAD) && (dec_cur.cmd.opcode != CMD_NOP) && !stalled && !branched )
    {
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
    if(!stalled && branched)
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
    	if (exe_cur.cmd.src1==wb_cur.pipe.cmd.dst&&(mem_cur.pipe.cmd.dst!=wb_cur.pipe.cmd.dst))
    		exe_cur.src1Val = wb_cur.alu_mem_result;
    	if(exe_cur.cmd.src2==wb_cur.pipe.cmd.dst && wb_cur.pipe.cmd.isSrc2Imm==false &&mem_cur.pipe.cmd.dst!=wb_cur.pipe.cmd.dst)//src2 might be immediate equaling to register index
    		exe_cur.src2Val =wb_cur.alu_mem_result;
    	break;
    case 2:
    	if (exe_cur.cmd.src1==wb_cur.pipe.cmd.dst &&(mem_cur.pipe.cmd.dst!=wb_cur.pipe.cmd.dst))//src2 might be immediate equaling to register index
    		exe_cur.src1Val = wb_cur.alu_mem_result;
    	if(exe_cur.cmd.src2==wb_cur.pipe.cmd.dst&& wb_cur.pipe.cmd.isSrc2Imm==false&&mem_cur.pipe.cmd.dst!=wb_cur.pipe.cmd.dst)
    		exe_cur.src2Val =wb_cur.alu_mem_result;
        break;
    case 3:
    	break;
    case 4:
    	if(wb_next.pipe.cmd.opcode==CMD_LOAD&&wb_next.pipe.src1Val+wb_next.pipe.src2Val==wb_cur.pipe.src2Val+Core.regFile[wb_cur.pipe.cmd.dst]){
    		Core.regFile[wb_next.pipe.cmd.dst] = wb_cur.pipe.src1Val;
    		if (exe_cur.cmd.src1==wb_next.pipe.cmd.dst)
    			exe_cur.src1Val=wb_cur.pipe.src1Val;
    		if (exe_cur.cmd.src2==wb_next.pipe.cmd.dst&&exe_cur.cmd.isSrc2Imm==false)
    			exe_cur.src2Val=wb_cur.pipe.src1Val;
    	}
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


