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

int main() {
    pthread_t thread1ID, thread2ID, thread3ID, thread4ID, thread5ID;
    scheduler sched;
    initscr();
    WINDOW * outputWin = newwin(50, 80, 3, 2);
    WINDOW * threadWin = newwin(25, 80, 3, 85);
    WINDOW * semaDumpWin = newwin(25, 80, 28, 85);
    box(outputWin, 0, 0);
    box(threadWin, 0, 0);
    box(semaDumpWin, 0, 0);
    mvwprintw(outputWin, 2, 18, "ULTIMA 2.0 (Phase 1: Scheduler and Semaphore)");
    mvwprintw(threadWin, 2, 10, "Window controlled by semaphore");
    mvwprintw(semaDumpWin, 2, 10, "Semaphore Output Info");
    wrefresh(outputWin);
    wrefresh(threadWin);
    wrefresh(semaDumpWin);
    sleep(5);
    sched.start(outputWin);

    int task1 = sched.create_task("File Explorer", outputWin);
    int task2 = sched.create_task("Task Manager", outputWin);
    int task3 = sched.create_task("Chrome", outputWin);
    int task4 = sched.create_task("Steam", outputWin);
    int task5 = sched.create_task("Firefox", outputWin);
    int task6 = sched.create_task("VIRUS", outputWin);
    /*
    sched.kill_task(2);
    sched.set_state(3, "RUNNING");
    sched.dump(outputWin);
    */
    int thread1 = pthread_create(&thread1ID, nullptr, thread1Fun, threadWin);
    int thread2 = pthread_create(&thread2ID, nullptr, thread2Fun, threadWin);
    int thread3 = pthread_create(&thread3ID, nullptr, thread3Fun, threadWin);
    int thread4 = pthread_create(&thread4ID, nullptr, thread4Fun, threadWin);
    int thread5 = pthread_create(&thread5ID, nullptr, thread5Fun, threadWin);

    sleep(5);
    endwin();
}

void* thread1Fun(void* arg) {
    WINDOW* win = (WINDOW*)arg;
    mvwprintw(win, 5, 1, "Hello from thread 1");
    wrefresh(win);
    return nullptr;
}
void* thread2Fun(void* arg) {
    WINDOW* win = (WINDOW*)arg;
    mvwprintw(win, 6, 1, "Hello from thread 2");
    wrefresh(win);
    return nullptr;
}
void* thread3Fun(void* arg) {
    WINDOW* win = (WINDOW*)arg;
    mvwprintw(win, 7, 1, "Hello from thread 3");
    wrefresh(win);
    return nullptr;
}
void* thread4Fun(void* arg) {
    WINDOW* win = (WINDOW*)arg;
    mvwprintw(win, 8, 1, "Hello from thread 4");
    wrefresh(win);
    return nullptr;
}
void* thread5Fun(void* arg) {
    WINDOW* win = (WINDOW*)arg;
    mvwprintw(win, 9, 1, "Hello from thread 5");
    wrefresh(win);
    return nullptr;
}