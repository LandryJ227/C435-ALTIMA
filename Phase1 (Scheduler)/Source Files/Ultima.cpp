#include "Ultima.h"
#include "scheduler.h"
#include "Sema.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <ncurses.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>


using namespace std;

void* thread1Fun(void* arg);
void* thread2Fun(void* arg);
void* thread3Fun(void* arg);
void* thread4Fun(void* arg);
void* thread5Fun(void* arg);

struct ThreadArgs {
    WINDOW* win;
    semaphore* sem;
    int taskID;
};

int main() {
    pthread_t thread1ID, thread2ID, thread3ID, thread4ID, thread5ID;
    scheduler sched;
    semaphore screenSema(1, "threadWin", &sched);

    initscr();
    WINDOW * outputWinOuter = newwin(50, 80, 3, 2);
    WINDOW* outputWin = newwin(47, 78, 6, 3);
    WINDOW * threadWin = newwin(10, 80, 3, 85);
    WINDOW * schedDumpWin = newwin(15, 80, 13, 85);
    WINDOW * semaDumpWin = newwin(25, 80, 28, 85);
    box(outputWinOuter, 0, 0);
    box(threadWin, 0, 0);
    box(schedDumpWin, 0, 0);
    box(semaDumpWin, 0, 0);
    mvwprintw(outputWinOuter, 2, 18, "ULTIMA 2.0 (Phase 1: Scheduler and Semaphore)");
    mvwprintw(threadWin, 2, 25, "Window controlled by semaphore");
    mvwprintw(threadWin, 3, 25, "------------------------------");
    mvwprintw(schedDumpWin, 1, 20, "------------ PROCESS  TABLE ------------");
    mvwprintw(semaDumpWin, 1, 1, "Semaphore Output Info");
    wrefresh(outputWin);
    wrefresh(outputWinOuter);
    wrefresh(threadWin);
    wrefresh(schedDumpWin);
    wrefresh(semaDumpWin);
    sleep(5);
    sched.start(outputWin);



    int task1 = sched.create_task("File Explorer", outputWin);
    int task2 = sched.create_task("Task Manager", outputWin);
    int task3 = sched.create_task("Chrome", outputWin);
    int task4 = sched.create_task("Steam", outputWin);
    int task5 = sched.create_task("Firefox", outputWin);

    ThreadArgs args1{threadWin, &screenSema, task1};
    ThreadArgs args2{threadWin, &screenSema, task2};
    ThreadArgs args3{threadWin, &screenSema, task3};
    ThreadArgs args4{threadWin, &screenSema, task4};
    ThreadArgs args5{threadWin, &screenSema, task5};

    int task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);
    task6 = sched.create_task("VIRUS", outputWin);

    /*
    sched.kill_task(2);
    sched.set_state(3, "RUNNING");*/
    sched.dump(schedDumpWin);

    int thread1 = pthread_create(&thread1ID, nullptr, thread1Fun, &args1);
    int thread2 = pthread_create(&thread2ID, nullptr, thread2Fun, &args2);
    int thread3 = pthread_create(&thread3ID, nullptr, thread3Fun, &args3);
    int thread4 = pthread_create(&thread4ID, nullptr, thread4Fun, &args4);
    int thread5 = pthread_create(&thread5ID, nullptr, thread5Fun, &args5);

    sleep(5);

    pthread_join(thread1ID, nullptr);
    pthread_join(thread2ID, nullptr);
    pthread_join(thread3ID, nullptr);
    pthread_join(thread4ID, nullptr);
    pthread_join(thread5ID, nullptr);

    endwin();
}

void* thread1Fun(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    WINDOW* win = args->win;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    mvwprintw(win, 5, 30, "Hello from thread 1");
    wrefresh(win);
    sem->up();
    return nullptr;
}
void* thread2Fun(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    WINDOW* win = args->win;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    mvwprintw(win, 6, 30, "Hello from thread 2");
    wrefresh(win);
    sem->up();
    return nullptr;
}
void* thread3Fun(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    WINDOW* win = args->win;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    mvwprintw(win, 7, 30, "Hello from thread 3");
    wrefresh(win);
    sem->up();
    return nullptr;
}
void* thread4Fun(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    WINDOW* win = args->win;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    mvwprintw(win, 8, 30, "Hello from thread 4");
    wrefresh(win);
    sem->up();
    return nullptr;
}
void* thread5Fun(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    WINDOW* win = args->win;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    mvwprintw(win, 9, 30, "Hello from thread 5");
    wrefresh(win);
    sem->up();
    return nullptr;
}