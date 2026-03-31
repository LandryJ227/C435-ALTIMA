#include "scheduler.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <time.h>
#include "Queue.h"


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
    cout << "..........SCHEDULING STARTED..........\n";
    //task_table[0].start_time = clock();
    //task_table[0].state = RUNNING;
    current_task = 0;
    set_quantum(1000 / MAX_TASKS);
    sleep(3);
}
//#########################################################
void scheduler::yield() {
    int counter = 0;
    tcb* currentTCB = process_table;                //start at front of table
    while (currentTCB->task_id != current_task) {   //traverse until we find current task
        currentTCB = currentTCB->next;
    }
    cout << "Current Task #" << current_task << " is trying to Yield." << endl;
    clock_t elapsed_time = clock() - get_start_time(current_task);
    cout << "Task: " << current_task << ". Elapsed time: " << elapsed_time << endl;
    cout << "Current Quantum: " << current_quantum << endl;

    if (elapsed_time >= current_quantum) {
        cout << "Yielding....(Switching from task #" << current_task << " to next ready task" << endl;
        if (currentTCB->state == RUNNING)
            currentTCB->state = READY;

        //current_task = sema_queue.peek();
        /*
        while (currentTCB->task_id != current_task && counter < MAX_TASKS-1) {
            currentTCB = (currentTCB->next == nullptr ? process_table : currentTCB->next);
            counter++;
        }
        */

        current_task = (current_task + 1) % MAX_TASKS;
        currentTCB = (currentTCB->next == nullptr ? process_table : currentTCB->next);//go to next task in tcb
        while (currentTCB->state != READY && counter < MAX_TASKS -1 ) {//while we find tasks that are not ready or go thru all tasks
            current_task = (current_task + 1) % MAX_TASKS;             //go to next task
            currentTCB = (currentTCB->next == nullptr ? process_table : currentTCB->next);
            counter ++;
        }

        if (counter < MAX_TASKS - 1 && currentTCB->task_id == current_task) {
            currentTCB->start_time = clock();
            currentTCB->state = RUNNING;
            cout << "Started Running Task #" << current_task << endl;
        }
        else {
            cout << "POSSIBLE DEAD LOCK" << endl;
        }
    }
    else cout << "NO Yield! (Task: "<< current_task << " still have some quantum left" << endl;
}

//#########################################################
int scheduler::create_task(string name) {
    cout << "Creating task #" << next_available_task_id << endl;
    sleep(1);
    if (next_available_task_id == MAX_TASKS) {      //check if exeeding max tasks
        cout << "FAILED: Available tasks exeeded." << endl;
        sleep(1);
        return (-1);                                //return -1 for error
    }

    tcb* newTask = new tcb();                       //new task
    newTask->next = nullptr;
    newTask->taskName = name;                       //set name of task
    newTask->state = READY;                       //set state of task
    newTask->start_time = clock();                  //set start time of task
    newTask->task_id = next_available_task_id++;    //inc next task id

    if (process_table == nullptr) {                 //if no tasks yet
        process_table = newTask;                    //process_table will point to this task
        cout << "Successfully created task #" << newTask->task_id << endl;
        sleep(1);
        return newTask->task_id;
    }

    tcb* ptrTCB = process_table;                    //point to front of process table
    while (ptrTCB->next != nullptr) {               //traverse until we find the end
        ptrTCB = ptrTCB->next;
    }
    ptrTCB->next = newTask;                      //add this new task to the end of process list

    cout << "Successfully created task #" << newTask->task_id << endl;
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

void scheduler::dump() {
    cout << "\n\nSCHEDULER DUMP" << endl;
    cout << "--------------" << endl;
    cout <<  "Quantum = " << current_quantum << endl;
    cout << "Task-ID\t Elapsed Time\tState" << endl;

    tcb* ptrTCB = process_table;

    while (ptrTCB != nullptr) {
        cout << ptrTCB->task_id << " \t" << ptrTCB->start_time << "\t\t" << ptrTCB->state << "\n";
        ptrTCB = ptrTCB->next;
        sleep(1);
    }
    cout << "----------------------------------------\n";
    sleep(5);
}