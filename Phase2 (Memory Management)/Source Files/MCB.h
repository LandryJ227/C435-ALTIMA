#ifndef ALTIMA_MCB_H
#define ALTIMA_MCB_H
#include "scheduler.h"
#include "IPC.h"

class MCB {
    scheduler Swapper;
    IPC Messenger;
};

#endif