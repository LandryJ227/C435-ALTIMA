//
// Created by jacob on 4/7/2026.
//

#include "MCB.h"

MCB::MCB(scheduler* mainSched, IPC* mainIPC) {
    sched = mainSched;
    messenger = mainIPC;
}