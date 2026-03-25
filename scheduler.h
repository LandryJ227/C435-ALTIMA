#include "tcb.h"
#include <cstrings>

struct tcb {
    int task_id;
    string state;
    clock_t start_time;
    tcb *next;
};

class scheduler {
    const string READY = "READY";
    const string RUNNING = "RUNNING";
    const string BLOCKED = "BLOCKED";
    const string DEAD = "DEAD";
    const int MAX_TASKS = 5;

    tcb *process_table;
    int numOfTasks=0;

    int current_task;
    long current_quantum;
    int next_available_task_id;
    tcb task_table[MAX_TASKS];

    scheduler();
    ~scheduler();
    void set_quantum(long quantum);
    long get_quantum();
    void set_state(int the_taskid, string the_state);
    string get_state(int the_taskid);
    int get_task_id();
    void start();
    int create_task();              // create appropriate data structures and calls coroutine()
    void kill_task(Task T);                // to kill a task (Set its status to DEAD)
    yield();                    // strict round robin process switch.
    void garbage_collect(int T_ID);          // remove dead task, free their resources, etc.
    dump(int level);            // debugging function with level indicating the verbosity of the
                                //dump include some functions which will allow you to dump the
                                //contents of the process table in a readable
                                //format. See the expected output section (below) for suggestions.


};