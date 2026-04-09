#ifndef ALTIMA_MCB_H
#define ALTIMA_MCB_H

class scheduler;
class IPC;

class MCB {
public:
    scheduler* sched;
    IPC* messenger;

    MCB(scheduler* mainSched, IPC* mainIPC) {
        sched = mainSched;
        messenger = mainIPC;
    }
};

#endif