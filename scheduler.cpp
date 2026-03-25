#include "scheduler.h"
#include "tcb.h"
#include <cstring>
#include <iostream>
using namespace std;

scheduler() {
    current_task = -1;
    next_available_task_id = 0;
    current_quantum = 300;
}
~scheduler() {
    cout << "Exiting scheduler..." << endl;
}
void set_quantum(long quantum) {
    current_quantum = quantum;
}
long get_quantum() {
    return (current_quantum);
}
void set_state(int the_taskid, string the_state) {
    task_table[the_taskid].state = the_state;
}
string get_state(int the_taskid) {
    return(task_table[the_taskid].state);
}
int get_task_id() {
    return current_task;
}
int create_task() {
    if (next_available_task_id < MAX_TASKS) {
        cout << "Creating task # " << next_available_task_id << endl;
        task_table[next_available_task_id].task_id = next_available_task_id;
        task_table[next_availavle_task_id].state = READY;
        task_table[next_availavle_task_id].next = NULL;

        next_available_task_id++;
        return(next_available_task_id - 1);
    }

    else {
        cout << "Create_task() FAILED: Available tasks exeeded. Max_TASKS = " << MAX_TASKS << endl;
        return (-1);
    }
}


















int scheduler::createTask(char name[64]) {
    Task newTask;
    strcpy(newTask.taskName,  name);
    strcpy(newTask.taskState, "BLOCKED");
    newTask.taskID = numOfTasks++;

    TCB ptrTCB = process_table;
    while (ptrTCB.next != null) {
        ptrTCB = ptrTCB.next;
    }
    ptrTCB.next = newTask;

    return newTask.taskID;
}

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
}