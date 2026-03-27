#include "Queue.h"
#include "Sched.h"
#include <iostream>
using namespace std;

void queue::enqueue(Task T) {
    if ((tail + 1) % QUEUE_SIZE != head) {
        taskQueue[tail] = T;
        tail = (tail + 1) % QUEUE_SIZE;
        numOfTasks++;
    }
    else cout << "ERROR: cannot enqueue into full queue" << endl;
}

Task queue::dequeue() {
    if (head != tail) {
        Task tempTask = taskQueue[head];
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
