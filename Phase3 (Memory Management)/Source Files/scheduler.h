#pragma once
#include <string>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <ncurses.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>
#include "Queue.h"
#include "message_queue.h"
#include "Sema.h"

using namespace std;
class semaphore;
class message_queue;

struct mailbox {
    message_queue* messageQueue;
    semaphore* mailSema;
    mailbox() : messageQueue(nullptr), mailSema(nullptr) {}
};

struct tcb {
    int task_id;
    string taskName;
    string state;
    clock_t start_time;
    tcb *next;
    mailbox taskMailbox;
    int memory_handle = -1; // memory handle for MMU
};

const string READY = "READY";
const string RUNNING = "RUNNING";
const string BLOCKED = "BLOCKED";
const string DEAD = "DEAD";
static const int MAX_TASKS = 5;



class scheduler {
public:

    const string READY = "READY";
    const string RUNNING = "RUNNING";
    const string BLOCKED = "BLOCKED";
    const string DEAD = "DEAD";
    static const int MAX_TASKS = 5;

    tcb *process_table;
    int numOfTasks=0;
    int outputLine = 5;
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
    void start(WINDOW* win);
    int create_task(string name, WINDOW* win);
    void kill_task(int T_ID);
    void yield (WINDOW* win);
    void garbage_collect(int T_ID);

};