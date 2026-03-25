#include "scheduler.h"
#include "tcb.h"
#include <cstring>

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