#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "pin.H"
#include "CacheSimulator.cpp"


// Globals
FILE * trace;


// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr) {
    //fprintf(trace,"%p: R %p\n", ip, addr);
    mem_read((uint64_t)addr,0);
}


// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr) {
    //fprintf(trace,"%p: W %p\n", ip, addr);
    mem_write((uint64_t)addr,0);
}


//To call for instruction accesses.
VOID RecordInstruction (VOID * ip) {
    //fprintf(trace,"%p: START \n", ip);
    mem_read((uint64_t)ip,0);
}


// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v) {
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);
    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordInstruction,
            IARG_INST_PTR,
            IARG_END);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                    IARG_INST_PTR,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_END);
        }
        // Note that in some architectures a single memory operand can be 
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                    IARG_INST_PTR,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_END);
        }
    }
}


VOID Fini(INT32 code, VOID *v) {
    //fprintf(trace, "#eof\n");
    saveStats(trace);
    fclose(trace);
}


/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage() {
    PIN_ERROR("This tool prints a trace of a cache simulator.");
    return -1;
}


/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[]) {
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("cachetrace.out", "w");
    srand (time(NULL));
    read_inputs();

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}
