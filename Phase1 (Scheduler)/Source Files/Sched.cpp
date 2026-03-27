#include "Sched.h"
#include <cstring>
#include <iostream>
using namespace std;

const string READY = "READY";
const string RUNNING = "RUNNING";
const string BLOCKED = "BLOCKED";
const string STOPPED = "STOPPED";
const int MAX_TASKS = 3;

//Task Control Block Logic
struct tcb {
    int task_id;
    string state;
    clock_t start_time;
    tcb *next;
};

class Sched {
    void Sched() {
        int current_task = -1;
        int next_available_task_id = 0;
        int current_quantum = 300;
    }

    void ~Sched(){
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
    void start() {
        cout << ".........." << endl;
        cout << "..........SCHEDULING STARTED" << endl;
        cout << "..........\n" << endl;
        task_table[0].start_time = clock();
        task_table[0].state = RUNNING;
        current_task = 0;
        set_quantum(1000 / MAX_TASKS);
        sleep(1);
    }
    void yield() {
        int counter = 0;
        cout << "Current Task # " << current_task << " is trying to Yield" << endl;
        clock_t elapsed_time = clock() = task_table[current_task].start_time;
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
    void dump() {
        cout << "------------ PROCESS TABLE ------------" << endl;
        cout << "Quantum = " << current_quantum << endl;
        cout << "Task-ID\t Elapsed Time\tState" <, endl;
        for (int i=0; i < MAX_TASKS; i++) {
            clock_t elapsed_time = clock() - task.table[i].start_time;
            printf("%6\t%8d\t%s", task_table[i].task_id, elapsed_time, task_table[i].state.c_str() );
            if (i == current_task)
                cout << " <-- CURRENT PROCESS";
            cout << endl;
        }
        cout << "----------------------------------------------------------------------\n" << endl;
    }



                cout << "POSSIBLE DEAD LOCK" << endl;
            }
        }
        else cout << "NO Yield! (Task: " << current_task << " Still have some quantum left)" << endl;
    }
    void dump() {
        cout << "------------ PROCESS TABLE ------------" << endl;
        cout << "Quantum = " << current_quantum << endl;
        cout << "Task-ID\t Elapsed Time\tState" <, endl;
        for (int i=0; i < MAX_TASKS; i++) {
            clock_t elapsed_time = clock() - task.table[i].start_time;
            printf("%6\t%8d\t%s", task_table[i].task_id, elapsed_time, task_table[i].state.c_str() );
            if (i == current_task)
                cout << " <-- CURRENT PROCESS";
            cout << endl;
        }
        cout << "----------------------------------------------------------------------\n" << endl;
    }
















    int Sched::createTask(char name[64]) {
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

    void Sched::kill_task(Task T) {
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













    int Sched::createTask(char name[64]) {
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

    void Sched::kill_task(Task T) {
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
}