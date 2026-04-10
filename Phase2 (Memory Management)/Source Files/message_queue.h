#pragma once
#ifndef C435_ALTIMA_MESSAGE_QUEUE_H
#define C435_ALTIMA_MESSAGE_QUEUE_H
#include "IPC.h"

class message_queue {
    public:
        static const int QUEUE_SIZE = 64; //Fixing max number of threads in the semaphore queue
         IPC::Message messageQueue[QUEUE_SIZE];  //Array to support a circular array
        //Index variables to support circular array data structure
        int size=0;
        int head=0;
        int tail=0;


        //Function to add an element to the back of the queue
        void enqueue(IPC::Message T);
        //Function to remove and return the integer valoue at the front of the queue.
        IPC::Message dequeue();
        //Checking if there are elements within the queue.
        bool isEmpty();
        //Formatted display of the content within the queue.
        //todo!
};

#endif //C435_ALTIMA_MESSAGE_QUEUE_H
