#ifndef ALTIMA_MCB_H
#define ALTIMA_MCB_H
#pragma once
class scheduler;
class IPC;

class MCB {
public:
    scheduler* sched;
    IPC* messenger;
    //Add any other important semaphores or structures you want to access between structures

    MCB(scheduler* mainSched, IPC* mainIPC);
};

#endif