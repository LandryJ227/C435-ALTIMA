#pragma once
#include "MCB.h"
#include "scheduler.h"
#include "IPC.h"

MCB::MCB(scheduler* mainSched, IPC* mainIPC) {
    sched = mainSched;
    messenger = mainIPC;
}