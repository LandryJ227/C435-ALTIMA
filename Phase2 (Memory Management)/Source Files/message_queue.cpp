#include "message_queue.h"
#include <iostream>
#include "IPC.h"
using namespace std;
//Function to add an element to the back of the queue
void message_queue::enqueue(IPC::Message T) {
    //Updating queue within a circular queue while supporting shift of queue.
    if ((tail + 1) % QUEUE_SIZE != head) {
        messageQueue[tail] = T;
        tail = (tail + 1) % QUEUE_SIZE;
        size++;
    }
    else cout << "ERROR: cannot enqueue into full queue" << endl;
}

//Function to remove and return to move the front
IPC::Message message_queue::dequeue() {
    IPC::Message holdMessage = messageQueue[head];
    if (head != tail) {
        //Updating queue within a circular queue while supporting shift of queue.
        head = (head + 1) % QUEUE_SIZE;
        size--;
        return holdMessage;
    }
    else {
        cout << "ERROR: cannot dequeue from empty queue" << endl;
        return holdMessage; //empty Message
    }
}

//Checking if there are elements within the queue.
bool message_queue::isEmpty() {
    if (head == tail) return true;
    else return false;
}

//Formatted display of the content within the queue.
//todo!
//void message_queue::printQueue();