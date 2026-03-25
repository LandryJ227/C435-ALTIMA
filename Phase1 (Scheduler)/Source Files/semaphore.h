#include "queue.h"
#include "tcb.h"

class semaphore {
    char resource_name [64];    // the name of the resource being managed
    int sema_value = 1;             // 0 or 1 in the case of a binary semaphore
    queue *sema_queue;

    void down(Task T);          // get the resource or get queued!
    void up();                  // release the resource
    void dump(int level);       // include some functions which will allow you to
                                // dump the contents of the semaphore in a readable format.
                                // See the expected output section (below) for suggestions.
};
