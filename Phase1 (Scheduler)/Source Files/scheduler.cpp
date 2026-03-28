#include "scheduler.h"
#include "tcb.h"
#include <cstring>
#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;

scheduler::scheduler() {
    current_task = -1;
    next_available_task_id = 0;
    current_quantum = 300;
}
scheduler::~scheduler() {
    cout << "Exiting scheduler..." << endl;
}
void scheduler::set_quantum(long quantum) {
    current_quantum = quantum;
}
long scheduler::get_quantum() {
    return (current_quantum);
}
void scheduler::set_state(int the_taskid, string the_state) {
    task_table[the_taskid].state = the_state;
}
string scheduler::get_state(int the_taskid) {
    return(task_table[the_taskid].state);
}
int scheduler::get_task_id() {
    return current_task;
}
/*
int scheduler::create_task() {
    if (next_available_task_id < MAX_TASKS) {
        cout << "Creating task # " << next_available_task_id << endl;
        task_table[next_available_task_id].task_id = next_available_task_id;
        task_table[next_available_task_id].state = READY;
        task_table[next_available_task_id].next = NULL;

        next_available_task_id++;
        return(next_available_task_id - 1);
    }

    else {
        cout << "Create_task() FAILED: Available tasks exeeded. Max_TASKS = " << MAX_TASKS << endl;
        return (-1);
    }
}*/
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
void scheduler::yield() {
    int counter = 0;
    cout << "Current Task # " << current_task << " is trying to Yield" << endl;
    clock_t elapsed_time = clock() - task_table[current_task].start_time;
    cout << "Task: " << current_task << ", Elapsed time: " << elapsed_time << endl;
    cout << "Current Quantum: " << current_quantum << endl;

    if (elapsed_time >= current_quantum) {
        cout << "Yielding....(Switching from task #" << current_task << " to next ready task)" << endl;
        if (task_table[current_task].state == RUNNING)
            task_table[current_task].state = READY;

        current_task = (current_task + 1) % MAX_TASKS;
        while (task_table[current_task].state != READY && counter < MAX_TASKS -1 ) {
            current_task = (current_task + 1) % MAX_TASKS;
            counter ++;
        }

        if (counter < MAX_TASKS - 1 && task_table[current_task].state == READY) {
            task_table[current_task].start_time = clock();
            task_table[current_task].state = RUNNING;
            cout << "Started Running Task #" << current_task << endl;
        }
        else {
            cout << "POSSIBLE DEAD LOCK" << endl;
        }
    }
    else cout << "NO Yield! (Task: " << current_task << " Still have some quantum left)" << endl;
}
















int scheduler::create_task(string name) {
    cout << "Entering createTask()" << endl;

    tcb* newTask = new tcb();
    newTask->taskName = name;            //set name of task
    newTask->state = "BLOCKED";//change later?
    newTask->task_id = numOfTasks++;              //inc total number of tasks

    if (process_table == nullptr) {
        process_table = newTask;
        cout << "Exiting createTask(), returning taskID = " << newTask->task_id << endl;
        return newTask->task_id;
    }

    tcb* ptrTCB = process_table;                 //point to front of process table
    while (ptrTCB->next != nullptr) {               //traverse until we find the end
        ptrTCB = ptrTCB->next;
    }
    ptrTCB->next = newTask;                      //add this new task to the end of process list

    cout << "Exiting createTask(), returning taskID = " << newTask->task_id << endl;
    return newTask->task_id;                      //return the task id
}
/*
void scheduler::kill_task(Task T) {
    T.taskState = "DEAD";
}

void garbage_collect(int T_ID) {
    TCB *ptrTCB = process_table;
    TCB* prevTCB = process_table;

    if (process_table->task.taskID == T_ID) {
        process_table = process_table->next;
        return;
    }

    while (ptrTCB->task.taskID != T_ID) {
        prevTCB = ptrTCB;
        ptrTCB = ptrTCB->next;
    }
    prevTCB->next = ptrTCB->next;
}*/

void scheduler::dump() {
    cout << "\n------------ PROCESS TABLE ------------" << endl;
    cout << "Quantum = " << current_quantum << endl;
    cout << "Task-ID\t Elapsed Time\tState" << endl;

    tcb* ptrTCB = process_table;
    while (ptrTCB != nullptr) {
        cout << ptrTCB->task_id << "\t" << ptrTCB->start_time << "\t" << ptrTCB->state << "\n";
        ptrTCB = ptrTCB->next;
    }

    cout << "---------------------------------------------\n" << endl;
}
