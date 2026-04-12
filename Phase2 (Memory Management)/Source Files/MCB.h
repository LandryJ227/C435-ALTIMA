#ifndef ALTIMA_MCB_H
#define ALTIMA_MCB_H
#include "Sched.h"
#include "IPC.h"

class MCB {
public:
    scheduler sched;
    IPC* messenger;
    //Add any other important semaphores or structures you want to access between structures

    MCB(scheduler* mainSched, IPC* mainIPC);
};

#endif