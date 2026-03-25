#include "semaphore.h"
#include "queue.h"
#include "tcb.h"
#include <iostream>
using namespace std;

void semaphore::down(Task T) {
    if (sema_value == 1) {
        sema_value = 0;
        //proceed
    }
    else {
        sema_queue->enqueue(T);
        //block this process until 1
    }
}

void semaphore::up() {
    if (sema_queue->isEmpty()) {                    //check if queue is empty
        sema_value = 1;                             //if it is, free the semaphore
    }
    else {
        Task nextTask = sema_queue->dequeue();
        //run this task next
    }
}

void semaphore::dump (int level) {
    cout << "\n##### Semaphore Dump #####" << endl;
    cout << "Resource name: " << resource_name << endl;
    cout << "Sema_value: " << sema_value << endl;
    cout << "Sema_queue: " << sema_queue << endl;
}
