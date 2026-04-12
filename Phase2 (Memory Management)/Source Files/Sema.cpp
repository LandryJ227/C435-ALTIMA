#include "Queue.h"
#include <iostream>
#include "Sema.h"
#include <string>
#include <pthread.h>
#include "Ultima.h"
#include "scheduler.h"


using namespace std;

semaphore::semaphore(int starting_value, string name, scheduler *theScheduler) {
    sema_value = starting_value;
    resource_name = name;
    lucky_task = -1;
    sched_ptr = theScheduler;
}

semaphore::~semaphore() {
    //Destroy.
}

void semaphore::down(int taskID, WINDOW* win, WINDOW* dumpWin) {
    pthread_mutex_lock(&mutex);
    if (taskID == lucky_task) { //taskID already obtained resource.
        snprintf(tempStr, sizeof(tempStr), "Task %d already has the resource! Ignore request.", lucky_task);
        write_window(win, outputWriteLine++, 12, tempStr);
        box(win, 0 , 0);
        wrefresh(win);
        dump(1, dumpWin);
    }
    else {
        if (sema_value >= 1) {
            sema_value--;
            lucky_task = taskID;
            dump(1, dumpWin);
        }
        else {
            sema_queue.enqueue(taskID);
            sched_ptr->set_state(taskID, "BLOCKED");
            snprintf(tempStr, sizeof(tempStr), "Block: %d and place into semaphore queue", taskID);
            write_window(win, outputWriteLine++, 12, tempStr);
            box(win, 0 , 0);
            wrefresh(win);
            dump(1, dumpWin);
            while (taskID != lucky_task) {
                pthread_cond_wait(&cond, &mutex);
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void semaphore::up(WINDOW* win, WINDOW* dumpWin)
{
    pthread_mutex_lock(&mutex);
    int task_id;
    snprintf(tempStr, sizeof(tempStr), "TaskID: %d, LuckID: %d", sched_ptr->get_task_id(), lucky_task);
    write_window(win, outputWriteLine++, 12, tempStr);
    box(win, 0 , 0);
    wrefresh(win);

    if(sched_ptr->get_task_id() == lucky_task)
    {
        if(sema_queue.isEmpty()) {
            sema_value++;
            lucky_task = -1;
            dump(1, dumpWin);
        }
        else {
            task_id = sema_queue.dequeue();
            sched_ptr->set_state(task_id, READY);
            snprintf(tempStr, sizeof(tempStr), "UnBlock: %d and release from the queue", task_id);
            write_window(win, outputWriteLine++, 12, tempStr);
            lucky_task = task_id;
            snprintf(tempStr, sizeof(tempStr), "Luck Task = %d", lucky_task);
            write_window(win, outputWriteLine++, 12, tempStr);
            box(win, 0 , 0);
            wrefresh(win);
            dump(1, dumpWin);
            pthread_cond_signal(&cond);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void semaphore::dump(int level, WINDOW* win)
{
    wclear(win);
    write_window(win, 1, 15, "--- Semaphore Dump Win ---");
    switch(level) {
        case 0:
            snprintf(tempStr, sizeof(tempStr), "Sema_Value: %d", sema_value);
            write_window(win, 3, 1, tempStr);
            snprintf(tempStr, sizeof(tempStr), "Sema_Name: %s", resource_name.c_str());
            write_window(win, 4, 1, tempStr);
            snprintf(tempStr, sizeof(tempStr), "Obtained by Task-ID: %d", lucky_task);
            write_window(win, 5, 1, tempStr);
            break;
        case 1:
            snprintf(tempStr, sizeof(tempStr), "Sema_Value: %d", sema_value);
            write_window(win, 3, 1, tempStr);
            snprintf(tempStr, sizeof(tempStr), "Sema_Name: %s", resource_name.c_str());
            write_window(win, 4, 1, tempStr);
            snprintf(tempStr, sizeof(tempStr), "Obtained by Task-ID: %d", lucky_task);
            write_window(win, 5, 1, tempStr);
            write_window(win, 6, 1, "Sema-Queue: ");
            //sema_queue.printQueue();
            break;
        default:
            write_window(win, 3, 1, "ERROR in SEMAPHORE DUMP level");

    }
    box(win, 0 , 0);
    wrefresh(win);
    //cout << "---------------------------------------" << endl;

}