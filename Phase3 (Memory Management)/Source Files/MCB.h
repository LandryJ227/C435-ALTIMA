#ifndef ALTIMA_MCB_H
#define ALTIMA_MCB_H
#pragma once

class scheduler;
class IPC;
class mmu;

class MCB {
public:
    scheduler* sched;
    IPC* ipc;
    mmu* mem;

    //Add any other important semaphores or structures you want to access between structures

    MCB(scheduler* mainSched, IPC* mainIPC, mmu* mainMem);
};

#endif