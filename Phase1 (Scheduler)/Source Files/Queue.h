#pragma once
#include "tcb.h"

class queue {
public:
    static const int QUEUE_SIZE = 64; //Fixing max number of threads in the semaphore queue
    int taskQueue[QUEUE_SIZE];  //Array to support a circular array
    //Index variables to support circular array data structure
    int numOfTasks=0;
    int head=0;
    int tail=0;

    //Function to add an element to the back of the queue
    void enqueue(int T);
    //Function to remove and return the integer valoue at the front of the queue.
    int dequeue();
    //Checking if there are elements within the queue.
    bool isEmpty();
    //Formatted display of the content within the queue.
    void printQueue();
};
