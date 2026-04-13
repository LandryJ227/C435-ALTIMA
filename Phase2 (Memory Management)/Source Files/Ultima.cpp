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

void* thread0Fun(void* arg);
void* thread1Fun(void* arg);
void* thread2Fun(void* arg);

struct ThreadArgs {
    semaphore* threadWinSem;
    semaphore* messWinSem;
    int taskID;
    MCB* mcb;
};
scheduler sched;
semaphore screenSema(1, "threadWin", &sched);
semaphore messSema(1, "messWin", &sched);
IPC ipc;

int main() {
    pthread_mutex_init(&winMutex, nullptr);//init mutex

    MCB mcb(&sched, &ipc);
    ipc.setMCB(&mcb);

    initscr();//start ncurses
    //------------------- CREATE WINDOWS -------------------
    outputWin = create_window(58, 55, 1, 1);
    threadWin = create_window(13, 55, 1, 57);
    schedWin = create_window(17, 55, 1, 113);
    semaWin = create_window(17, 55, 18, 113);
    messWin = create_window(18, 70, 35, 113);
    thread0Win = create_window(15, 55, 14, 57);
    thread1Win = create_window(15, 55, 29, 57);
    thread2Win = create_window(15, 55, 44, 57);

    //------------------- LABEL WINDOWS -------------------
    write_window(outputWin, 1,17, "--- Output Window ---");
    write_window(schedWin, 1, 15, "--- Scheduler Dump Win ---");
    write_window(semaWin, 1, 15, "--- Semaphore Dump Win ---");
    write_window(messWin, 1, 16, "--- Message Dump Win ---");
    write_window(thread0Win, 1, 15, "--- Thread 0 Win ---");
    write_window(thread1Win, 1, 15, "--- Thread 1 Win ---");
    write_window(thread2Win, 1, 15, "--- Thread 2 Win ---");
    write_window(threadWin, 1, 15, "--- Shared Win ---");

    pthread_t thread0ID, thread1ID, thread2ID;

    sched.start(outputWin);

    //create tasks
    outputWriteLine++;
    write_window(outputWin, outputWriteLine++,1, "First we will create 3 tasks:\n");
    write_window(outputWin, outputWriteLine++,1, "-----------------------------\n");
    int task0 = sched.create_task("File Explorer", outputWin);
    int task1 = sched.create_task("Task Manager", outputWin);
    int task2 = sched.create_task("Chrome", outputWin);
    int task3 = sched.create_task("Steam", outputWin);
    //output process table
    sched.dump(schedWin);
    //kill a task
    outputWriteLine++;
    write_window(outputWin, outputWriteLine++,1, "Now we will kill the 4th task created");
    write_window(outputWin, outputWriteLine++,1, "-------------------------------------");
    sched.kill_task(task3);
    //output process table after kill
    sched.dump(schedWin);
    //create threads
    ThreadArgs args0{ &screenSema, &messSema, task0, &mcb};
    ThreadArgs args1{ &screenSema, &messSema, task1, &mcb};
    ThreadArgs args2{&screenSema, &messSema, task2, &mcb};
    int thread0 = pthread_create(&thread0ID, nullptr, thread0Fun, &args0);
    int thread1 = pthread_create(&thread1ID, nullptr, thread1Fun, &args1);
    int thread2 = pthread_create(&thread2ID, nullptr, thread2Fun, &args2);
    pthread_join(thread0ID, nullptr);
    pthread_join(thread1ID, nullptr);
    pthread_join(thread2ID, nullptr);
    sleep(1000);
    endwin();
    return(0);
}

//functions for each thread
//####################################################################################################
void* thread0Fun(void* arg) {
    int thread0outputLine=3;
    write_window(thread0Win, thread0outputLine++, 1, "Starting Thread 0...");
    sleep(5);
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* threadWinSem = args->threadWinSem;
    semaphore* messWinSem = args->messWinSem;
    int taskID = args->taskID;
    MCB* mcb = args->mcb;
    char buf[256];

    write_window(thread0Win, thread0outputLine++, 1, "Attempting to write to shared win...");
    sleep(5);
    threadWinSem->down(taskID, outputWin, semaWin);
    sched.dump(schedWin);
    sched.yield(outputWin);
    write_window(threadWin, 4,8, "-----------------------");
    write_window(threadWin, 5,8, "| Hello from thread 0 |");
    write_window(threadWin, 6,8, "-----------------------");
    write_window(thread0Win, thread0outputLine++, 1, "Successfully wrote to shared win!");
    sched.dump(schedWin);
    threadWinSem->up(outputWin, semaWin);


    write_window(thread0Win, thread0outputLine++, 1, "Looking for incoming message(s)...");
    sleep(5);
    messWinSem->down(taskID, outputWin, semaWin);
    mcb->ipc->Message_Print(0, messWin);
    messWinSem->up(outputWin, semaWin);
    Message receivingMessage;
    int messageIn = mcb->ipc->Message_Receive(0, &receivingMessage, semaWin, outputWin);
    while (messageIn != -1) {
        snprintf(buf, sizeof(buf), "Received: %s", receivingMessage.Msg_Text);
        write_window(thread0Win, thread0outputLine++, 1, buf);
        //sleep(2);
        messageIn = mcb->ipc->Message_Receive(0, &receivingMessage, semaWin, outputWin);
    }

    write_window(thread0Win, thread0outputLine++, 1, "Sending message to thread 2...");
    sleep(1);
    string messTemp = "From T_0: Hello Thread 2!";
    Message sendingMessage(0, 2, 0, (char*)messTemp.c_str());
    mcb->ipc->Message_Send(&sendingMessage, outputWin, semaWin);
    write_window(thread0Win, thread0outputLine++, 1, "Successfully sent!");

    sched.dump(schedWin);
    write_window(thread0Win, thread0outputLine++, 1, "Ending Thread 0");
    sleep(2);
    return nullptr;
}
//####################################################################################################
void* thread1Fun(void* arg) {
    int thread1outputLine=3;
    write_window(thread1Win, thread1outputLine++, 1, "Starting Thread 1...");
    sleep(5);
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* threadWinSem = args->threadWinSem;
    semaphore* messWinSem = args->messWinSem;
    int taskID = args->taskID;
    MCB* mcb = args->mcb;
    char buf[256];
    
    write_window(thread1Win, thread1outputLine++, 1, "Sending message to thread 0...");
    sleep(1);
    string messTemp = "From T_1: Hello Thread 0!";
    Message sendingMessage(1, 0, 0, (char*)messTemp.c_str());
    mcb->ipc->Message_Send(&sendingMessage, outputWin, semaWin);
    write_window(thread1Win, thread1outputLine++, 1, "Successfully sent!");

    int count = mcb->ipc->Message_Count(0);
    snprintf(buf, sizeof(buf), "Mailbox[0] Count After Send: %d", count);
    write_window(thread1Win, thread1outputLine++, 1, buf);

    write_window(thread1Win, thread1outputLine++, 1, "Attempting to write to shared win...");
    sleep(5);

    threadWinSem->down(taskID, outputWin, semaWin);
    sched.dump(schedWin);
    sched.yield(outputWin);
    write_window(threadWin, 4,8, "-----------------------");
    write_window(threadWin, 5,8, "| Hello from thread 1 |");
    write_window(threadWin, 6,8, "-----------------------");
    write_window(thread1Win, thread1outputLine++, 1, "Successfully wrote to shared win!");
    sched.dump(schedWin);
    threadWinSem->up(outputWin, semaWin);

    write_window(thread1Win, thread1outputLine++, 1, "Sending message to thread 2...");
    sleep(1);
    messTemp = "From T_1: Hello Thread 2!";
    Message sendingMessage2(1, 2, 0, (char*)messTemp.c_str());
    mcb->ipc->Message_Send(&sendingMessage2, outputWin, semaWin);
    write_window(thread1Win, thread1outputLine++, 1, "Successfully sent!");

    count = mcb->ipc->Message_Count(2);
    snprintf(buf, sizeof(buf), "Mailbox[2] Count After Send: %d", count);
    write_window(thread1Win, thread1outputLine++, 1, buf);

    write_window(thread1Win, thread1outputLine++, 1, "Looking for incoming message(s)...");

    count = mcb->ipc->Message_Count(1);
    snprintf(buf, sizeof(buf), "Mailbox[1] Count Before Receive: %d", count);
    write_window(thread1Win, thread1outputLine++, 1, buf);

    sleep(5);
    messWinSem->down(taskID, outputWin, semaWin);
    mcb->ipc->Message_Print(1, messWin);
    messWinSem->up(outputWin, semaWin);
    Message receivingMessage;
    int messageIn = mcb->ipc->Message_Receive(1, &receivingMessage, semaWin, outputWin);
    while (messageIn != -1) {
        snprintf(buf, sizeof(buf), "Received: %s", receivingMessage.Msg_Text);
        write_window(thread1Win, thread1outputLine++, 1, buf);
        //sleep(2);
        messageIn = mcb->ipc->Message_Receive(1, &receivingMessage, semaWin, outputWin);
    }

    count = mcb->ipc->Message_Count(1);
    snprintf(buf, sizeof(buf), "Mailbox[1] Count After Receive: %d", count);
    write_window(thread1Win, thread1outputLine++, 1, buf);

    write_window(thread1Win, thread1outputLine++, 1, "Ending Thread 1");
    sleep(2);
    return nullptr;
}
void* thread2Fun(void* arg) {
    int thread2outputLine=3;
    write_window(thread2Win, thread2outputLine++, 1, "Starting Thread 2...");
    sleep(5);
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* threadWinSem = args->threadWinSem;
    semaphore* messWinSem = args->messWinSem;

    int taskID = args->taskID;
    MCB* mcb = args->mcb;
    char buf[256];

    write_window(thread2Win, thread2outputLine++, 1, "Sending message to thread 0...");
    sleep(1);
    string messTemp = "From T_2: Hello Thread 0!";
    Message sendingMessage(2, 0, 0, (char*)messTemp.c_str());
    mcb->ipc->Message_Send(&sendingMessage, outputWin, semaWin);
    write_window(thread2Win, thread2outputLine++, 1, "Successfully sent!");

    int count = mcb->ipc->Message_Count(0);
    snprintf(buf, sizeof(buf), "Mailbox[0] Count After Send: %d", count);
    write_window(thread2Win, thread2outputLine++, 1, buf);

    write_window(thread2Win, thread2outputLine++, 1, "Attempting to write to shared win...");
    sleep(5);

    threadWinSem->down(taskID, outputWin, semaWin);
    sched.dump(schedWin);
    sched.yield(outputWin);
    write_window(threadWin, 4,8, "-----------------------");
    write_window(threadWin, 5,8, "| Hello from thread 2 |");
    write_window(threadWin, 6,8, "-----------------------");
    write_window(thread2Win, thread2outputLine++, 1, "Successfully wrote to shared win!");
    sched.dump(schedWin);
    threadWinSem->up(outputWin, semaWin);

    write_window(thread2Win, thread2outputLine++, 1, "Looking for incoming message(s)...");

    count = mcb->ipc->Message_Count(2);
    snprintf(buf, sizeof(buf), "Mailbox[2] Count Before Receive: %d", count);
    write_window(thread2Win, thread2outputLine++, 1, buf);

    sleep(6);
    messWinSem->down(taskID, outputWin, semaWin);
    mcb->ipc->Message_Print(2, messWin);
    messWinSem->up(outputWin, semaWin);
    Message receivingMessage;
    int messageIn = mcb->ipc->Message_Receive(2, &receivingMessage, semaWin, outputWin);
    while (messageIn != -1) {
        snprintf(buf, sizeof(buf), "Received: %s", receivingMessage.Msg_Text);
        write_window(thread2Win, thread2outputLine++, 1, buf);
        //sleep(2);
        messageIn = mcb->ipc->Message_Receive(2, &receivingMessage, semaWin, outputWin);
    }

    count = mcb->ipc->Message_Count(2);
    snprintf(buf, sizeof(buf), "Mailbox[2] Count After Receive: %d", count);
    write_window(thread2Win, thread2outputLine++, 1, buf);

    write_window(thread2Win, thread2outputLine++, 1, "Ending Thread 2");
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





