#include "Queue.h"
#include <iostream>
#include "Sema.h"
#include <string>
#include <ncurses.h>
#include <pthread.h>

using namespace std;

semaphore::semaphore(int starting_value, string name, scheduler *theScheduler) {
    pthread_mutex_init(&mutex, nullptr);
    sema_value = starting_value;
    resource_name = name;
    lucky_task = -1;

    sched_ptr = theScheduler;
}

semaphore::~semaphore() {
    pthread_mutex_destroy(&mutex);
    //sema_queue.Reset();
    //pthread_mutex_destroy(&mutex);
}

void semaphore::down(int taskID, WINDOW* win, WINDOW* schedWin)
{
    pthread_mutex_lock(&mutex);

    if (taskID == lucky_task) {
        mvwprintw(win, outputLine++, 2, "Task #%d already has the resource! Ignore request.", lucky_task);
        dump(1, win);
    }
    else {
        if (sema_value >= 1) {
            sema_value--;
            lucky_task = taskID;
            dump(1, win);
        }
        else {
            sema_queue.enqueue(taskID);
            sched_ptr->set_state(taskID, "BLOCKED");
            mvwprintw(win, outputLine++, 2, "Block : %d and place into semaphore queue",  taskID);
            dump(1, win);

            sched_ptr->yield(schedWin, sema_queue);
            dump(1, win);
        }
    }
    pthread_mutex_unlock(&mutex);
    wrefresh(win);
}

void semaphore::up(WINDOW* win, WINDOW* schedWin)
{
    int task_id;
    pthread_mutex_lock(&mutex);
    mvwprintw(win, outputLine++, 2, "TaskID : %d, LuckID : %d", sched_ptr->get_task_id(), lucky_task);

    if(sched_ptr->get_task_id() == lucky_task)
    {
        if(sema_queue.isEmpty()) {
            sema_value++;
            lucky_task = -1;
            dump(1, win);
        }
        else {
            task_id = sema_queue.dequeue();
            sched_ptr->set_state(task_id, READY);
            mvwprintw (win, outputLine++, 3, "UnBlock  : %d and release from the queue", task_id);
            lucky_task = task_id;
            mvwprintw (win, outputLine++, 3, "Luck Task = %d", lucky_task);
            dump(1, win);
            sched_ptr->yield(schedWin, sema_queue);
            dump(1, win);
        }
    }
    pthread_mutex_unlock(&mutex);
    wrefresh(win);
}

void semaphore::dump(int level, WINDOW* win)
{/*
    cout << "-----SEMAPHORE DUMP-------" << endl;
    switch(level) {
        case 0:
            cout << "Sema_Value: " << sema_value << endl;
            cout << "Sema_Name : " << resource_name << endl;
            cout << "Obtained by Task-ID: " << lucky_task<< endl;
            break;
        case 1:
            cout << "Sema_Value         : " << sema_value << endl;
            cout << "Sema_Name          : " << resource_name << endl;
            cout << "Obtained by Task-ID: " << lucky_task << endl;
            cout << "Sema-Queue: " << endl;
            sema_queue.printQueue();
            break;
        default:
            cout << "ERROR in SEMAPHORE DUMP level";
    }
    cout << "---------------------------------------" << endl;*/

}