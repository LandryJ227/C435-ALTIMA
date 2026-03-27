class Ultima_Queue{
private:
    static const int QUEUE_SIZE = 64;
    int taskQueue[QUEUE_SIZE];
    int numOfTasks=0;
    int head=0;
    int tail=0;

public:
    Ultima_Queue();
    ~Ultima_Queue();
    void enqueue(int task_ID);
    int dequeue();
    bool isEmpty();
};
