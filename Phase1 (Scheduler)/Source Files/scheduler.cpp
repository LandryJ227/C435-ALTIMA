#include "scheduler.h"
#include "tcb.h"
#include <cstring>
#include <string>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <time.h>
#include <ncurses.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>


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
void scheduler::start(WINDOW* win) {
    mvwprintw(win, 3, 20, "..........SCHEDULING STARTED..........\n");
    wrefresh(win);
    //task_table[0].start_time = clock();
    //task_table[0].state = RUNNING;
    current_task = 0;
    set_quantum(1000 / MAX_TASKS);
    sleep(3);
}
//#########################################################
void scheduler::yield(WINDOW* win) {
    int counter = 0;
    tcb* currentTCB = process_table;
    while (currentTCB->task_id != current_task) {
        currentTCB = currentTCB->next;
    }
    mvwprintw(win, outputLine++, 20, "Current Task #%d is trying to Yield.", current_task);
    clock_t elapsed_time = clock() - get_start_time(current_task);
    mvwprintw(win, outputLine++, 20, "Task: %d, Elapsed time: %d", current_task, elapsed_time);
    mvwprintw(win, outputLine++, 20, "Current Quantum: %d",  current_quantum);

    if (elapsed_time >= current_quantum) {
        mvwprintw(win, outputLine++, 20, "Yielding....(Switching from task #%d to next ready task", current_task);
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
            mvwprintw(win, outputLine++, 20, "Started Running Task #%d", current_task);
        }
        else {
            mvwprintw(win, outputLine++, 20, "POSSIBLE DEAD LOCK");
        }
    }
    else mvwprintw(win, outputLine++, 20, "NO Yield! (Task: %d still have some quantum left", current_task);
}
//#########################################################
int scheduler::create_task(string name, WINDOW* win) {
    int maxLines = getmaxy(win) - 2;
    scrollok(win, TRUE);
    if (outputLine >= maxLines) {
        scroll(win);
        scroll(win);
        scroll(win);
        //box(win, 0, 0);
        outputLine = maxLines-3;
    }
    mvwprintw(win, outputLine++, 20, "Creating task #%d...", next_available_task_id);
    wrefresh(win);
    sleep(1);
    if (next_available_task_id == MAX_TASKS) {      //check if exeeding max tasks
        mvwprintw(win, outputLine++, 20, "FAILED: Available tasks exeeded.");
        wrefresh(win);
        outputLine++;
        sleep(1);
        return (-1);                                //return -1 for error
    }

    tcb* newTask = new tcb();                       //new task
    newTask->next = nullptr;
    newTask->taskName = name;                       //set name of task
    newTask->state = "READY";                       //set state of task
    newTask->start_time = clock();                  //set start time of task
    newTask->task_id = next_available_task_id++;    //inc next task id

    if (process_table == nullptr) {                 //if no tasks yet
        process_table = newTask;                    //process_table will point to this task
        mvwprintw(win, outputLine++, 20, "Successfully created task #%d", newTask->task_id);
        wrefresh(win);
        outputLine++;
        sleep(1);
        return newTask->task_id;
    }

    tcb* ptrTCB = process_table;                    //point to front of process table
    while (ptrTCB->next != nullptr) {               //traverse until we find the end
        ptrTCB = ptrTCB->next;
    }
    ptrTCB->next = newTask;                      //add this new task to the end of process list

    mvwprintw(win, outputLine++, 20, "Successfully created task #%d", newTask->task_id);
    wrefresh(win);
    outputLine++;
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
void scheduler::dump(WINDOW* win) {
    int line = 3;
    mvwprintw(win, line++, 20,  "Quantum = %ld", current_quantum);
    line++;
    mvwprintw(win, line++, 20,  "Task-ID\t Elapsed Time\tState");

    tcb* ptrTCB = process_table;

    while (ptrTCB != nullptr) {
        mvwprintw(win, line++, 20, "   %d\t\t%d\t%s", ptrTCB->task_id, ptrTCB->start_time, ptrTCB->state.c_str());
        ptrTCB = ptrTCB->next;
        sleep(1);
    }
    mvwprintw(win, line, 20, "----------------------------------------");
    wrefresh(win);
    sleep(5);
}