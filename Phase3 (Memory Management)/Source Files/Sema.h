#pragma once
#include "Queue.h"
#include <pthread.h>
#include <ncurses.h>
#include <string>
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
    void down(int taskID, WINDOW* win, WINDOW* dumpWin);
    void up(WINDOW* win, WINDOW* dumpWin);
    void dump(int level, WINDOW* win);
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};