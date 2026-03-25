#include "semaphore.h"
#include "queue.h"
#include "task.h"
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
    sema_value = 1;
    //wake up next process
}

void semaphore::dump (int level) {
    cout << "\n##### Semaphore Dump #####" << endl;
    cout << "Resource name: " << resource_name << endl;
    cout << "Sema_value: " << sema_value << endl;
    cout << "Sema_queue: " << sema_queue << endl;
}
