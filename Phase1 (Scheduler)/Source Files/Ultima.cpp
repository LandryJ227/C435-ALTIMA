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

void* thread1Fun(void* arg);
void* thread2Fun(void* arg);

struct ThreadArgs {
    semaphore* sem;
    int taskID;
};
scheduler sched;
semaphore screenSema(1, "threadWin", &sched);

int main() {
    pthread_t thread1ID, thread2ID;

    sched.start();

    //create tasks
    int task1 = sched.create_task("File Explorer");
    int task2 = sched.create_task("Task Manager");
    sched.dump();
    //sched.kill_task(1);
    //sched.dump();
    //int task3 = sched.create_task("Email");
    //sched.dump();
    //output process table

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
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    sched.dump();
    //call down to check semaphore
    cout << "Hello from thread 1" << endl;
    sched.dump();
    sem->up();
    sleep(2);
    return nullptr;
}
void* thread2Fun(void* arg) {
    //initialize variables and objects needed
    ThreadArgs* args = (ThreadArgs*)arg;
    semaphore* sem = args->sem;
    int taskID = args->taskID;

    sem->down(taskID);
    sched.dump();
    //call down to check semaphore
    cout << "Hello from thread 2" << endl;
    sched.dump();
    sem->up();
    sleep(2);
    return nullptr;
}
