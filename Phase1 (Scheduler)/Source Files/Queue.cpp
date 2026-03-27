#include "Queue.h"
#include "Sched.h"
#include <iostream>
using namespace std;

void Ultima_Queue::enqueue(int task) {
    if ((tail + 1) % QUEUE_SIZE != head) {
        taskQueue[tail] = task;
        tail = (tail + 1) % QUEUE_SIZE;
        numOfTasks++;
    }
    else cout << "ERROR: cannot enqueue into full queue" << endl;
}

tcb Ultima_Queue::dequeue() {
    if (head != tail) {
        int tempTask = taskQueue[head];
        head = (head + 1) % QUEUE_SIZE;
        numOfTasks--;
        return tempTask;
    }
    else {
        cout << "ERROR: cannot dequeue from empty queue" << endl;
    }
}

bool Ultima_Queue::isEmpty() {
    if (head == tail) return true;
    else return false;
}
