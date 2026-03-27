#include "Queue.h"
#include "sched.h"
#include <iostream>
#include "Sema.h"


using namespace std;


string resource_name;
int sema_value;
int lucky_task;

queue<int> sema_queue;
scheduler *sched_ptr;

public:

semaphore(int starting_value, string name, scheduler *theScheduler) {
    sema_value = starting_value;
    resource_name = name;
    lucky_task = -1;

    sched_ptr = theScheduler;
}

~semaphore() {
    //sema_queue.Reset();
    //pthread_mutex_destroy(&mutex);
}

void down(int taskID)
{

    if(taskID == lucky_task) {
        cout << "Task # " << lucky_task << " already has the resource! Ignore request." << endl;
        dump(1);
    }
    else {
        if(sema_value >= 1) {
            sema_value--;
            lucky_task = taskID;
            dump(1);

            sched_ptr->yield();
            dump(1);
        }
    }
}

void up()
{
    int task_id;
    cout << "TaskID : " <<  sched_ptr -> get_task_id() << ", LuckID : " << lucky_task << endl;
    if(sched_ptr->get_task_id() == lucky_task)
    {
        if(sema_queue.isEmpty()) {
            sema_value++;
            lucky_task = -1;
            dump(1);
        }
        else {
            task_id = sema_queue.De_Q();
            sched_ptr->set_state(task_id, READY);
            cout << "UnBlock  : " << task_id << " and release from the queue" << endl;
            lucky_task = task_id;
            cout << "Luck Task = " << lucky_task << endl;
            dump(1);
            sched_ptr->yield();
            dump(1);
        }
    }
}

void dump(int level)
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
            sema_queue.Print();
            break;
        default:
            cout << "ERROR in SEMAPHORE DUMP level";
    }
    cout << "---------------------------------------" << endl;
}

            