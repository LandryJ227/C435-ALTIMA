#pragma once
#include "Queue.h"
#include "scheduler.h"
#include <pthread.h>
#include <ncurses.h>
using namespace std;

class scheduler;

class semaphore {
    string resource_name;
    int sema_value = 1;
    int lucky_task;
    queue sema_queue;
    scheduler *sched_ptr;
    pthread_mutex_t mutex;
    int outputLine=3;
public:
    semaphore(int starting_value, string name, scheduler *theScheduler);
    ~semaphore();

    void down(int taskID, WINDOW* win, WINDOW* schedWin);
    void up(WINDOW* win, WINDOW* schedWin);
    void dump(int level, WINDOW* win);
};