#pragma once
#include "tcb.h"

class queue {
public:
    static const int QUEUE_SIZE = 64;
    int taskQueue[QUEUE_SIZE];
    int numOfTasks=0;
    int head=0;
    int tail=0;

    void enqueue(int T);
    int dequeue();
    bool isEmpty();
    void printQueue();
};
