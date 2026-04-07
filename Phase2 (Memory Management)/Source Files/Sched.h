#include <iostream>
#include <ctime>

#ifndef SCHEDULER_H
#define SCHEDULER_H

const std::string READY = "READY";
const std::string RUNNING = "RUNNING";
const std::string BLOCKED = "BLOCKED";
const std::string DEAD = "DEAD";
static const int MAX_TASKS = 3;

struct tcb {
    int task_id;
    std::string state;
    std::clock_t start_time;
    tcb *next;
};

class scheduler {
private:
    int current_task;
    long current_quantum;
    int next_available_task_id;
    tcb task_table[MAX_TASKS];

    tcb *process_table;
    int numOfTasks = 0;
public:
    scheduler();
    ~scheduler();
    void set_quantum(long quantum);
    long get_quantum();
    void set_state(int the_taskid, std::string the_state);
    std::string get_state(int the_taskid);
    int get_task_id();
    void start();
    int create_task();                  // create appropriate data structures and calls coroutine()
    void yield();                            //defers control logic back to scheduler
    void dump();
    void kill_task(tcb T);             // to kill a task (Set its status to STOPPED)
    void garbage_collect(int T_ID);     // remove dead task, free their resources, etc.
};
#endif