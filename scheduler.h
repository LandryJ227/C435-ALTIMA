#include "tcb.h"

class scheduler {
    TCB *process_table;
    int numOfTasks=0;

    int create_task();              // create appropriate data structures and calls coroutine()
    void kill_task(Task T);                // to kill a task (Set its status to DEAD)
    yield();                    // strict round robin process switch.
    void garbage_collect(int T_ID);          // remove dead task, free their resources, etc.
    dump(int level);            // debugging function with level indicating the verbosity of the
                                //dump include some functions which will allow you to dump the
                                //contents of the process table in a readable
                                //format. See the expected output section (below) for suggestions.
};
