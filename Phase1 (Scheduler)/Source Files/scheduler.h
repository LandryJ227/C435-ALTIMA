#pragma once
#include "tcb.h"
#include <string>
#include <ctime>
#include "Ultima.h"
#include "scheduler.h"
#include "Sema.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

#include <assert.h>
#include <time.h>
#include <ncurses.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

struct tcb {
    int task_id;
    string taskName;
    string state;
    clock_t start_time;
    tcb *next;
};

const string READY = "READY";
const string RUNNING = "RUNNING";
const string BLOCKED = "BLOCKED";
const string DEAD = "DEAD";
static const int MAX_TASKS = 5;

class scheduler {
public:



    tcb *process_table;
    int numOfTasks=0;

    int current_task;
    long current_quantum;
    int next_available_task_id=0;
    tcb task_table[MAX_TASKS];

    void dump(WINDOW* win);
    scheduler();
    ~scheduler();
    void set_quantum(long quantum);
    long get_quantum();
    void set_state(int the_taskid, string the_state);
    string get_state(int the_taskid);
    clock_t get_start_time(int T_ID);
    int get_task_id();
    void start();
    int create_task(string name);              // create appropriate data structures and calls coroutine()
    void kill_task(int T_ID);                // to kill a task (Set its status to DEAD)
    void yield();                    // strict round robin process switch.
    void garbage_collect(int T_ID);          // remove dead task, free their resources, etc.
    //dump(int level);            // debugging function with level indicating the verbosity of the
                                //dump include some functions which will allow you to dump the
                                //contents of the process table in a readable
                                //format. See the expected output section (below) for suggestions.
};