#include "Queue.h"
#include <iostream>
#include "Sema.h"
#include <string>
#include <pthread.h>

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

void semaphore::down(int taskID){
    if (taskID == lucky_task) { //taskID already obtained resource.
        cout << "Task " << lucky_task <<" already has the resource! Ignore request." << endl;
        dump(1);
    }
    else {
        if (sema_value >= 1) {
            sema_value--;
            lucky_task = taskID;
            dump(1);
        }
        else {
            sema_queue.enqueue(taskID);
            sched_ptr->set_state(taskID, "BLOCKED");
            "Block : " << taskID << " and place into semaphore queue" << endl;
            dump(1);

            sched_ptr->yield(sema_queue);
            dump(1);
        }
    }
}

void semaphore::up();
{
    int task_id;
    pthread_mutex_lock(&mutex);
    cout <<  "TaskID : %d, LuckID : %d" <<  sched_ptr->get_task_id() << lucky_task;

    if(sched_ptr->get_task_id() == lucky_task)
    {
        if(sema_queue.isEmpty()) {
            sema_value++;
            lucky_task = -1;
            dump(1);
        }
        else {
            task_id = sema_queue.dequeue();
            sched_ptr->set_state(task_id, READY);
            cout << "UnBlock  : " << task_id << " and release from the queue" << endl;
            lucky_task = task_id;
            cout << "Luck Task = " << lucky_task << endl;
            dump(1);
            sched_ptr->yield(sema_queue);
            dump(1);
        }
    }
    //pthread_mutex_unlock(&mutex);  --Mutex?
}

void semaphore::dump(int level)
{
    cout << "-----SEMAPHORE DUMP-------" << endl;
    switch(level) {
        case 0:
            cout << "Sema_Value: " << sema_value << endl;
            cout << "Sema_Name : " << resource_name << endl;
            cout << "Obtained by Task-ID: " << lucky_task<< endl;
            break;
        case 1:
            cout << "Sema_Value         : " << sema_value << endl;
            cout << "Sema_Name          : " << resource_name << endl;
            cout << "Obtained by Task-ID: " << lucky_task << endl;
            cout << "Sema-Queue: " << endl;
            sema_queue.printQueue();
            break;
        default:
            cout << "ERROR in SEMAPHORE DUMP level";
    }
    cout << "---------------------------------------" << endl;

}