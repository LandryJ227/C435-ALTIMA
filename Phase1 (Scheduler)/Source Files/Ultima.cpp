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
    cout << "First we will create 3 tasks:\n";
    cout << "-----------------------------\n";
    int task1 = sched.create_task("File Explorer");
    int task2 = sched.create_task("Task Manager");
    int task3 = sched.create_task("Chrome");
    //output process table
    sched.dump();
    //kill a task
    cout << "Now we will kill the 3rd task created:\n";
    cout << "--------------------------------------\n";
    sched.kill_task(task3);
    //output process table after kill
    sched.dump();

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
    sched.dump();
    sched.yield();
    cout << "\n-----------------------" << endl;
    cout << "| Hello from thread 1 |" << endl;
    cout << "-----------------------" << endl;
    sched.dump();


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

    sched.dump();
    sched.yield();
    cout << "\n-----------------------" << endl;
    cout << "| Hello from thread 2 |" << endl;
    cout << "-----------------------" << endl;
    sched.dump();

    sem->up();
    cout << "\n-----------------------End of Thread 2-----------------------\n\n" << endl;
    sleep(2);
    return nullptr;
}
