#include "scheduler.h"
#include "Sema.h"
#include "Ultima.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ncurses.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>
using namespace std;

void* thread1Fun(void* arg);
void* thread2Fun(void* arg);




struct ThreadArgs {
    semaphore* sem;
    int taskID;
};
scheduler sched;
semaphore screenSema(1, "threadWin", &sched);

int main() {
    initscr();//start ncurses
    //------------------- CREATE WINDOWS -------------------
    outputWin = create_window(51, 55, 1, 1);
    threadWin = create_window(17, 55, 1, 57);
    schedWin = create_window(17, 55, 1, 113);
    semaWin = create_window(17, 55, 18, 113);
    messWin = create_window(17, 55, 35, 113);

    //------------------- LABEL WINDOWS -------------------
    write_window(outputWin, 1,17, "--- Output Window ---");
    write_window(schedWin, 1, 15, "--- Scheduler Dump Win ---");
    write_window(semaWin, 1, 15, "--- Semaphore Dump Win ---");
    write_window(messWin, 1, 16, "--- Message Dump Win ---");

    pthread_t thread1ID, thread2ID;

    sched.start(outputWin);

    //create tasks
    outputWriteLine++;
    write_window(outputWin, outputWriteLine++,12, "First we will create 3 tasks:\n");
    write_window(outputWin, outputWriteLine++,12, "-----------------------------\n");
    int task1 = sched.create_task("File Explorer", outputWin);
    int task2 = sched.create_task("Task Manager", outputWin);
    int task3 = sched.create_task("Chrome", outputWin);
    //output process table
    sched.dump(schedWin);
    //kill a task
    outputWriteLine++;
    write_window(outputWin, outputWriteLine++,12, "Now we will kill the 3rd task created");
    write_window(outputWin, outputWriteLine++,12, "-------------------------------------");
    sched.kill_task(task3);
    //output process table after kill
    sched.dump(schedWin);
    sleep(1000);
    //create threads
    ThreadArgs args1{ &screenSema, task1};
    ThreadArgs args2{ &screenSema, task2};
    int thread1 = pthread_create(&thread1ID, nullptr, thread1Fun, &args1);
    int thread2 = pthread_create(&thread2ID, nullptr, thread2Fun, &args2);
    pthread_join(thread1ID, nullptr);
    pthread_join(thread2ID, nullptr);

}

//functions for each thread
void* thread1Fun(void* arg) {
    cout << "\n\n-----------------------Start of Thread 1-----------------------" << endl;
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    sched.dump(schedWin);
    sched.yield();
    cout << "\n-----------------------" << endl;
    cout << "| Hello from thread 1 |" << endl;
    cout << "-----------------------" << endl;
    sched.dump(schedWin);


    sem->up();
    cout << "\n-----------------------End of Thread 1-----------------------\n\n" << endl;
    sleep(2);
    return nullptr;
}
void* thread2Fun(void* arg) {
    cout << "\n\n-----------------------Start of Thread 2-----------------------" << endl;
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);

    sched.dump(schedWin);
    sched.yield();
    cout << "\n-----------------------" << endl;
    cout << "| Hello from thread 2 |" << endl;
    cout << "-----------------------" << endl;
    sched.dump(schedWin);

    sem->up();
    cout << "\n-----------------------End of Thread 2-----------------------\n\n" << endl;
    sleep(2);
    return nullptr;
}


WINDOW *create_window(int height, int width, int starty, int startx) {
    WINDOW *Win;
    Win = newwin(height, width, starty, startx);
    scrollok(Win, TRUE); // Allow scrolling of the window
    scroll(Win); // scroll the window
    box(Win, 0 , 0); // 0, 0 gives default characters
    // for the vertical and horizontal lines
    wrefresh(Win); // draw the window
    return Win;
}
//----------------------------------------------------------------
void write_window(WINDOW * Win, const char* text)
{
    wprintw(Win, text);
    box(Win, 0 , 0);
    wrefresh(Win); // draw the window
}
//----------------------------------------------------------------
void write_window(WINDOW * Win, int y, int x, const char* text)
{
    mvwprintw(Win, y, x, text);
    box(Win, 0 , 0);
    wrefresh(Win); // draw the window
}
//----------------------------------------------------------------





