#include "task.h"

class queue {
public:
    static const int QUEUE_SIZE = 64;
    Task taskQueue[QUEUE_SIZE];
    int numOfTasks=0;
    int head=0;
    int tail=0;

    void enqueue(Task T);
    Task dequeue();
};