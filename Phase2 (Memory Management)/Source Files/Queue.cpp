#include "Queue.h"
#include <iostream>
using namespace std;

//Function to add an element to the back of the queue
void queue::enqueue(int T) {
    //Updating queue within a circular queue while supporting shift of queue.
    if ((tail + 1) % QUEUE_SIZE != head) {
        taskQueue[tail] = T;
        tail = (tail + 1) % QUEUE_SIZE;
        numOfTasks++;
    }
    else cout << "ERROR: cannot enqueue into full queue" << endl;
}

//Function to remove and return the integer valoue at the front of the queue.
int queue::dequeue() {
    if (head != tail) {
        //Updating queue within a circular queue while supporting shift of queue.
        int tempTask = taskQueue[head];
        head = (head + 1) % QUEUE_SIZE;
        numOfTasks--;
        return tempTask;
    }
    else {
        cout << "ERROR: cannot dequeue from empty queue" << endl;
        return -1;
    }
}

//Checking if there are elements within the queue.
bool queue::isEmpty() {
    if (head == tail) return true;
    else return false;
}

//Formatted display of the content within the queue.
string queue::printQueue() {
    //Empty case
    if (head == tail) {
        return "Queue is empty";
    }
    else {
        string output = "";
        //Printing from head to tail of the queue.
        int i = head;
        while (i != tail) {
            output += std::to_string(taskQueue[i]) + " ";
            i = (i + 1) % QUEUE_SIZE;   // wrap around
        }
        return output;
    }
}


int queue::peek() {
    return taskQueue[head];
}