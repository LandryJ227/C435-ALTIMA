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
#include "MCB.h"
#include "IPC.h"
#include "message.h"

using namespace std;

void* thread1Fun(void* arg);
void* thread2Fun(void* arg);




struct ThreadArgs {
    semaphore* sem;
    int taskID;
    MCB* mcb;
};
scheduler sched;
semaphore screenSema(1, "threadWin", &sched);
IPC ipc;

int main() {
    pthread_mutex_init(&winMutex, nullptr);//init mutex

    MCB mcb(&sched, &ipc);
    ipc.setMCB(&mcb);

    initscr();//start ncurses
    //------------------- CREATE WINDOWS -------------------
    outputWin = create_window(51, 55, 1, 1);
    threadWin = create_window(17, 55, 1, 57);
    schedWin = create_window(17, 55, 1, 113);
    semaWin = create_window(17, 55, 18, 113);
    messWin = create_window(17, 55, 35, 113);
    thread1Win = create_window(17, 55, 18, 57);
    thread2Win = create_window(17, 55, 35, 57);

    //------------------- LABEL WINDOWS -------------------
    write_window(outputWin, 1,17, "--- Output Window ---");
    write_window(schedWin, 1, 15, "--- Scheduler Dump Win ---");
    write_window(semaWin, 1, 15, "--- Semaphore Dump Win ---");
    write_window(messWin, 1, 16, "--- Message Dump Win ---");
    write_window(thread1Win, 1, 15, "--- Thread 1 Win ---");
    write_window(thread2Win, 1, 15, "--- Thread 2 Win ---");
    write_window(threadWin, 1, 15, "--- Shared Win ---");

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
    //create threads
    ThreadArgs args1{ &screenSema, task1, &mcb};
    ThreadArgs args2{ &screenSema, task2, &mcb};
    int thread1 = pthread_create(&thread1ID, nullptr, thread1Fun, &args1);
    int thread2 = pthread_create(&thread2ID, nullptr, thread2Fun, &args2);
    pthread_join(thread1ID, nullptr);
    pthread_join(thread2ID, nullptr);
    sleep(5);
    endwin();
    return(0);
}

//functions for each thread
void* thread1Fun(void* arg) {
    //-----------------------Start of Thread 1-----------------------
    write_window(thread1Win, 3, 1, "Starting Thread 1...");
    sleep(6);
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* sem = args->sem;
    int taskID = args->taskID;
    MCB* mcb = args->mcb;

    write_window(thread1Win, 4, 1, "Attempting to write to shared win...");
    sleep(6);
    sem->down(taskID, outputWin, semaWin);
    sched.dump(schedWin);
    sched.yield(outputWin);
    write_window(threadWin, 4,8, "-----------------------");
    write_window(threadWin, 5,8, "| Hello from thread 1 |");
    write_window(threadWin, 6,8, "-----------------------");

    write_window(thread1Win, 5, 1, "Sending message to thread 2...");
    sleep(6);
    string messTemp = "From thread 1: Hello Thread 2!";
    Message sendingMessage(0, 1, 0, (char*)messTemp.c_str());
    mcb->ipc->Message_Send(&sendingMessage, outputWin, semaWin);

    //Message receivingMessage;
    //mcb->ipc->Message_Receive(0, &receivingMessage, semaWin);
    //write_window(threadWin, 6,8, receivingMessage.Msg_Text);

    mcb->ipc->Message_Print(1, messWin);


    sched.dump(schedWin);
    sem->up(outputWin, semaWin);
    //cout << "\n-----------------------End of Thread 1-----------------------\n\n" << endl;
    write_window(thread1Win, 6, 1, "Ending Thread 1");
    sleep(2);
    return nullptr;
}
void* thread2Fun(void* arg) {
    //cout << "\n\n-----------------------Start of Thread 2-----------------------" << endl;
    write_window(thread2Win, 3, 1, "Starting Thread 2...");
    sleep(6);
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* sem = args->sem;
    int taskID = args->taskID;
    MCB* mcb = args->mcb;

    write_window(thread2Win, 4, 1, "Attempting to write to shared win...");
    sleep(6);
/*
    string messTemp = "Hello thread 1, from thread 2";
    Message sendingMessage2(1, 2, 0, (char*)messTemp.c_str());
    mcb->ipc->Message_Send(&sendingMessage2, outputWin, semaWin);
*/
    sem->down(taskID, outputWin, semaWin);
    sched.dump(schedWin);
    sched.yield(outputWin);
    write_window(threadWin, 4,8, "-----------------------");
    write_window(threadWin, 5,8, "| Hello from thread 2 |");
    write_window(threadWin, 6,8, "-----------------------");


    write_window(thread2Win, 5, 1, "Looking for incoming message(s)...");
    sleep(6);

    Message receivingMessage;
    mcb->ipc->Message_Receive(1, &receivingMessage, semaWin);
    write_window(threadWin, 6,8, receivingMessage.Msg_Text);

    sched.dump(schedWin);
    sem->up(outputWin, semaWin);
    //cout << "\n-----------------------End of Thread 2-----------------------\n\n" << endl;
    write_window(thread2Win, 6, 1, "Ending Thread 2");
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
    pthread_mutex_lock(&winMutex);
    mvwprintw(Win, y, x, text);
    box(Win, 0 , 0);
    wrefresh(Win); // draw the window
    pthread_mutex_unlock(&winMutex);
}
//----------------------------------------------------------------





