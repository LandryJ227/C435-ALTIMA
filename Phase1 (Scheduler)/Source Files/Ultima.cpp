#include "scheduler.h"
#include "Sema.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ncurses.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>
using namespace std;
/*
struct ThreadArgs {
    WINDOW* win;
    semaphore* sem;
    int taskID;
};*/


int main() {
    pthread_t thread1ID, thread2ID;
    scheduler sched;
    //semaphore screenSema(1, "threadWin", &sched);

    sched.start();

    //create tasks
    int task1 = sched.create_task("File Explorer");
    int task2 = sched.create_task("Task Manager");
    sched.dump();
    sched.kill_task(1);
    sched.dump();
    //output process table

    //create threads
    /*
    ThreadArgs args1{threadWin, &screenSema, task1};
    ThreadArgs args2{threadWin, &screenSema, task2};
    int thread1 = pthread_create(&thread1ID, nullptr, thread1Fun, &args1);
    int thread2 = pthread_create(&thread2ID, nullptr, thread2Fun, &args2);
    pthread_join(thread1ID, nullptr);
    pthread_join(thread2ID, nullptr);
*/
}
/*
//functions for each thread
void* thread1Fun(void* arg) {
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    WINDOW* win = args->win;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID, semaDumpWin, outputWin);
    sched.dump(schedDumpWin);
    //call down to check semaphore
    pthread_mutex_lock(&ncurses_mutex);
    //output to window
    mvwprintw(win, 4, 30, "Hello from thread 1");
    wrefresh(win);
    pthread_mutex_unlock(&ncurses_mutex);
    sleep(2);
    sem->up(semaDumpWin, outputWin);
    sched.dump(schedDumpWin);
    return nullptr;
}
*/