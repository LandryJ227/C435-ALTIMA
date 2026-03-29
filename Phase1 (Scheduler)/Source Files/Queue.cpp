#include "tcb.h"
#include "Queue.h"
#include <iostream>
using namespace std;

void queue::enqueue(int T) {
    if ((tail + 1) % QUEUE_SIZE != head) {
        taskQueue[tail] = T;
        tail = (tail + 1) % QUEUE_SIZE;
        numOfTasks++;
    }
    else cout << "ERROR: cannot enqueue into full queue" << endl;
}

int queue::dequeue() {
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

bool queue::isEmpty() {
    if (head == tail) return true;
    else return false;
}

void queue::printQueue() {
    if (head == tail) {
        cout << "Queue is empty" << endl;
    }
    else {
        for (int i = 0; i < QUEUE_SIZE; i++) {
            cout << taskQueue[i] << endl;
        }
    }
}
