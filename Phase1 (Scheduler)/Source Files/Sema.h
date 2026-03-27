#include "Queue.h"


class semaphore {
private:
    std::string name;    // the name of the resource being managed
    int sema_value = 1;             // 0 or 1 in the case of a binary semaphore
    Ultima_Queue *sema_queue;

public:
    semaphore(int starting_value, std::string name, scheduler *theSchedule);
    ~semaphore();
    void down(int T);          // get the resource or get queued!
    void up();                  // release the resource
    void dump(int level);       // include some functions which will allow you to
                                // dump the contents of the semaphore in a readable format.
                                // See the expected output section (below) for suggestions.
};
