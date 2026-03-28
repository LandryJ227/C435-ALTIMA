#include "Ultima.h"
#include "scheduler.h"
#include "Sema.h"

int main() {
    scheduler sched;
    int task1 = sched.create_task("File Explorer");
    int task2 = sched.create_task("Task Manager");
    int task3 = sched.create_task("Chrome");
    int task4 = sched.create_task("Steam");
    sched.dump();
}