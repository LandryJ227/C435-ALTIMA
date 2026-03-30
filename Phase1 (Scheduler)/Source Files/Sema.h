#pragma once
#include "Queue.h"
#include "scheduler.h"
#include <pthread.h>
using namespace std;

class scheduler;

class semaphore {
    string resource_name;
    int sema_value = 1;
    int lucky_task;
    queue sema_queue;
    scheduler *sched_ptr;

public:
    semaphore(int starting_value, string name, scheduler *theScheduler);
    ~semaphore();

    void down(int taskID);
    void up();
    void dump(int level);
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};