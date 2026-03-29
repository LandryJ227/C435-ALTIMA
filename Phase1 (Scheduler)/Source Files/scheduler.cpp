#include "scheduler.h"
#include "tcb.h"
#include <cstring>
#include <string>
#include <iostream>
#include <unistd.h>
#include <ctime>

#include <assert.h>
#include <time.h>
#include <ncurses.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>


using namespace std;
//#########################################################
scheduler::scheduler() {
    current_task = -1;
    next_available_task_id = 0;
    current_quantum = 300;
}
//#########################################################
scheduler::~scheduler() {
    cout << "Exiting scheduler..." << endl;
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
void scheduler::start() {
    cout << ".........." << endl;
    cout << "..........SCHEDULING STARTED" << endl;
    cout << "..........\n" << endl;
    task_table[0].start_time = clock();
    task_table[0].state = RUNNING;
    current_task = 0;
    set_quantum(1000 / MAX_TASKS);
    sleep(1);
}
//#########################################################
void scheduler::yield() {
    int counter = 0;
    tcb* currentTCB = process_table;
    while (currentTCB->task_id != current_task) {
        currentTCB = currentTCB->next;
    }

    cout << "Current Task # " << current_task << " is trying to Yield" << endl;
    clock_t elapsed_time = clock() - get_start_time(current_task);
    cout << "Task: " << current_task << ", Elapsed time: " << elapsed_time << endl;
    cout << "Current Quantum: " << current_quantum << endl;

    if (elapsed_time >= current_quantum) {
        cout << "Yielding....(Switching from task #" << current_task << " to next ready task)" << endl;
        if (currentTCB->state == RUNNING)
            currentTCB->state = READY;

        current_task = (current_task + 1) % MAX_TASKS;
        currentTCB = (currentTCB->next == nullptr ? process_table : currentTCB->next);
        while (currentTCB->state != READY && counter < MAX_TASKS -1 ) {
            current_task = (current_task + 1) % MAX_TASKS;
            currentTCB = (currentTCB->next == nullptr ? process_table : currentTCB->next);
            counter ++;
        }

        if (counter < MAX_TASKS - 1 && currentTCB->state == READY) {
            currentTCB->start_time = clock();
           currentTCB->state = RUNNING;
            cout << "Started Running Task #" << current_task << endl;
        }
        else {
            cout << "POSSIBLE DEAD LOCK" << endl;
        }
    }
    else cout << "NO Yield! (Task: " << current_task << " Still have some quantum left)" << endl;
}
//#########################################################
int scheduler::create_task(string name) {
    //cout << "Entering createTask()" << endl;
    if (next_available_task_id == MAX_TASKS) {      //check if exeeding max tasks
        //cout << "Create_task() FAILED: Available tasks exeeded. Max_TASKS = " << MAX_TASKS << endl;
        return (-1);                                //return -1 for error
    }

    tcb* newTask = new tcb();                       //new task
    newTask->taskName = name;                       //set name of task
    newTask->state = "READY";                       //set state of task
    newTask->start_time = clock();                  //set start time of task
    newTask->task_id = next_available_task_id++;    //inc next task id

    if (process_table == nullptr) {                 //if no tasks yet
        process_table = newTask;                    //process_table will point to this task
        //cout << "Exiting createTask(), returning taskID = " << newTask->task_id << endl;
        return newTask->task_id;
    }

    tcb* ptrTCB = process_table;                    //point to front of process table
    while (ptrTCB->next != nullptr) {               //traverse until we find the end
        ptrTCB = ptrTCB->next;
    }
    ptrTCB->next = newTask;                      //add this new task to the end of process list

    //cout << "Exiting createTask(), returning taskID = " << newTask->task_id << endl;
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
    mvwprintw(win, 1, 20, "------------ PROCESS TABLE ------------");
    mvwprintw(win, 4, 10,  "Quantum = %ld", current_quantum);
    mvwprintw(win, 7, 10,  "Task-ID\t Elapsed Time\tState");
    wrefresh(win);
    sleep(5);
    return;

    tcb* ptrTCB = process_table;
    while (ptrTCB != nullptr) {
        cout << ptrTCB->task_id << "\t "
             << (clock() - ptrTCB->start_time) << "\t\t"
             << ptrTCB->state << "\n";
        ptrTCB = ptrTCB->next;
        sleep(1);
    }
    cout << "---------------------------------------------\n" << endl;
}