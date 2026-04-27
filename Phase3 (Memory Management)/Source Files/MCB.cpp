#include "MCB.h"
#include "scheduler.h"
#include "IPC.h"
#include "mmu.h"

MCB::MCB(scheduler* mainSched, IPC* mainIPC, mmu* mainMem) {
    sched = mainSched;
    ipc = mainIPC;
    mem = mainMem;
}