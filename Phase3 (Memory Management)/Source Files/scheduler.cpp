#include "scheduler.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <time.h>
#include "Ultima.h"
#include "Queue.h"
#include "message_queue.h"

using namespace std;
//#########################################################
scheduler::scheduler() {
    process_table = nullptr;
    current_task = -1;
    next_available_task_id = 0;
    current_quantum = 300;
}
//#########################################################
scheduler::~scheduler() {
    //cout << "Exiting scheduler..." << endl;
}
//#########################################################
void scheduler::set_quantum(long quantum) {
    current_quantum = quantum;
}
//#########################################################
long scheduler::get_quantum() {
    return (current_quantum);
}
//#########################################################
void scheduler::set_state(int T_ID, string newState) {
    if (process_table->task_id == T_ID) {
        process_table->state = newState;
        return;
    }

    tcb* ptrTCB = process_table;
    while (ptrTCB->task_id != T_ID) {
        ptrTCB = ptrTCB->next;
    }
    ptrTCB->state = newState;
}
//#########################################################
string scheduler::get_state(int T_ID) {
        if (process_table->task_id == T_ID) {
            return process_table->state;
        }

        tcb* ptrTCB = process_table;
        while (ptrTCB->task_id != T_ID) {
            ptrTCB = ptrTCB->next;
        }
        return ptrTCB->state;
}
//#########################################################
clock_t scheduler::get_start_time(int T_ID) {
    if (process_table->task_id == T_ID) {
        return process_table->start_time;
    }

    tcb* ptrTCB = process_table;
    while (ptrTCB->task_id != T_ID) {
        ptrTCB = ptrTCB->next;
    }
    return ptrTCB->start_time;
}
//#########################################################
int scheduler::get_task_id() {
    return current_task;
}
//#########################################################
void scheduler::start(WINDOW* win) {
    write_window(win, outputWriteLine++,8, "..........SCHEDULING STARTED..........\n");
    //task_table[0].start_time = clock();
    //task_table[0].state = RUNNING;
    current_task = 0;
    set_quantum(1000 / MAX_TASKS);
    sleep(3);
}
//#########################################################
void scheduler::yield(WINDOW* win) {
    int counter = 0;
    tcb* currentTCB = process_table;                //start at front of table

    while (currentTCB->task_id != current_task && currentTCB->task_id != current_task) {   //traverse until we find current task
        currentTCB = currentTCB->next;
    }

    if (currentTCB == nullptr) {
        write_window(win, outputWriteLine++, 1, "Current task not found.");
        return;
    }
    snprintf(tempStr, sizeof(tempStr), "Current Task #%d is trying to Yield", current_task);
    write_window(win, outputWriteLine++, 1, tempStr);

    clock_t elapsed_time = clock() - get_start_time(current_task);
    snprintf(tempStr, sizeof(tempStr), "Task: %d. Elapsed time: %d.", current_task, elapsed_time);
    write_window(win, outputWriteLine++, 1, tempStr);
    snprintf(tempStr, sizeof(tempStr), "Current Quantum: %d", current_quantum);
    write_window(win, outputWriteLine++, 1, tempStr);

    if (elapsed_time < current_quantum) {
        snprintf(tempStr, sizeof(tempStr), "NO Yield! (Task: %d still has some quantum left)", current_task);
        write_window(win, outputWriteLine++, 1, tempStr);
    }

    snprintf(tempStr, sizeof(tempStr), "Yielding..(Switching from task #%d to next ready task)", current_task);
    write_window(win, outputWriteLine++, 1, tempStr);

    tcb* nextTCB = (currentTCB->next == nullptr ? process_table : currentTCB->next);

    while (nextTCB != currentTCB && nextTCB->state != "READY") {

        if (nextTCB->next == nullptr)
            nextTCB = process_table;
        else
            nextTCB = nextTCB->next;
        counter++;
    }

    if (nextTCB->state == "READY" && nextTCB != currentTCB) {
        currentTCB->state = "READY";
        current_task = nextTCB->task_id;
        nextTCB->start_time = clock();
        nextTCB->state = "RUNNING";
        snprintf(tempStr, sizeof(tempStr), "Started Running Task #%d", current_task);
        write_window(win, outputWriteLine++, 1, tempStr);
    } else {
        write_window(win, outputWriteLine++, 1, "No other READY task found.");
    }
    box(win, 0 , 0);
    wrefresh(win);
}

//#########################################################
int scheduler::create_task(string name, WINDOW* win) {
    snprintf(tempStr, sizeof(tempStr), "Creating task #%d...", next_available_task_id);
    write_window(win, outputWriteLine++, 1, tempStr);
    box(win, 0 , 0);
    wrefresh(win);
    sleep(1);
    if (next_available_task_id == MAX_TASKS) {      //check if exeeding max tasks
        write_window(win, outputWriteLine++,1,  "FAILED: Available tasks exeeded.");
        sleep(1);
        return (-1);                                //return -1 for error
    }

    tcb* newTask = new tcb();                       //new task
    newTask->next = nullptr;
    newTask->taskName = name;                       //set name of task
    newTask->state = READY;                       //set state of task
    newTask->start_time = clock();                  //set start time of task
    newTask->task_id = next_available_task_id++;    //inc next task id

    newTask->taskMailbox.mailSema = new semaphore(1, "mailbox", this);
    newTask->taskMailbox.messageQueue = new message_queue();

    if (process_table == nullptr) {                 //if no tasks yet
        process_table = newTask;                    //process_table will point to this task
        snprintf(tempStr, sizeof(tempStr), "Successfully created task #%d!", newTask->task_id);
        write_window(win, outputWriteLine++, 1, tempStr);
        box(win, 0 , 0);
        wrefresh(win);
        sleep(1);
        return newTask->task_id;
    }

    tcb* ptrTCB = process_table;                    //point to front of process table
    while (ptrTCB->next != nullptr) {               //traverse until we find the end
        ptrTCB = ptrTCB->next;
    }
    ptrTCB->next = newTask;                      //add this new task to the end of process list

    snprintf(tempStr, sizeof(tempStr), "Successfully created task #%d!", newTask->task_id);
    write_window(win, outputWriteLine++,1, tempStr);
    box(win, 0 , 0);
    wrefresh(win);
    sleep(1);
    return newTask->task_id;                      //return the task id
}
//#########################################################
void scheduler::kill_task(int T_ID) {
    if (process_table->task_id == T_ID) {
        process_table->state = DEAD;
        return;
    }

    tcb* ptrTCB = process_table;
    while (ptrTCB->task_id != T_ID) {
        ptrTCB = ptrTCB->next;
    }
    ptrTCB->state = DEAD;
    garbage_collect(T_ID);
}
//#########################################################
void scheduler::garbage_collect(int T_ID) {
    tcb *ptrTCB = process_table;
    tcb* prevTCB = process_table;

    if (process_table->task_id == T_ID) {
        process_table = process_table->next;
        return;
    }

    while (ptrTCB->task_id != T_ID) {
        prevTCB = ptrTCB;
        ptrTCB = ptrTCB->next;
    }
    prevTCB->next = ptrTCB->next;
}
//#########################################################

void scheduler::dump(WINDOW* win) {
    int count = 6;
    char buf[256];
    wclear(win);
    write_window(win, 1, 15, "--- Scheduler Dump Win ---");
    snprintf(buf, sizeof(buf), "Quantum = %d", current_quantum);
    write_window(win, 4, 4, buf);
    write_window(win, 5, 4,  "Task-ID  Task Name     Elapsed Time   State");
    tcb* ptrTCB = process_table;
    write_window(win, count++, 4, "-------------------------------------------");
    while (ptrTCB != nullptr) {
        snprintf(buf, sizeof(buf), "   %d     %s      ", ptrTCB->task_id, ptrTCB->taskName.c_str());
        write_window(win, count, 4, buf);
        snprintf(buf, sizeof(buf), "%d        %s", (clock() - ptrTCB->start_time), ptrTCB->state.c_str());
        write_window(win, count++, 33, buf);
        ptrTCB = ptrTCB->next;
    }
    write_window(win, count, 4, "-------------------------------------------");
    sleep(2);
}
