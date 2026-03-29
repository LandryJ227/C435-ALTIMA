#include "Ultima.h"
#include "scheduler.h"
#include "Sema.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    scheduler sched;
    int task1 = sched.create_task("File Explorer");
    int task2 = sched.create_task("Task Manager");
    int task3 = sched.create_task("Chrome");
    int task4 = sched.create_task("Steam");
    int task5 = sched.create_task("Firefox");
    int task6 = sched.create_task("VIRUS");
    sched.kill_task(2);
    sched.set_state(3, "RUNNING");
    cout << sched.get_state(3) << endl;
    sched.dump();
}