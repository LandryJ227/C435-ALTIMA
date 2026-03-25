class Task {
public:
    char taskName[64];
    char taskState[64];
    int taskID;
};

class TCB {
public:
    Task task;
    TCB *next;
};