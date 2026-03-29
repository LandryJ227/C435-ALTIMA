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

int main() {
    scheduler sched;
    initscr();
    WINDOW * outputWin = newwin(12, 80, 3, 2);
    box(outputWin, 0, 0);
    //mvwprintw(outputWin, 2, 28, "ULTIMA 2.0 (Phase 1)");
    wrefresh(outputWin);


    int task1 = sched.create_task("File Explorer");
    int task2 = sched.create_task("Task Manager");
    int task3 = sched.create_task("Chrome");
    int task4 = sched.create_task("Steam");
    int task5 = sched.create_task("Firefox");
    int task6 = sched.create_task("VIRUS");
    sched.kill_task(2);
    sched.set_state(3, "RUNNING");
    //cout << sched.get_state(3) << endl;
    sched.dump(outputWin);
    endwin();
}