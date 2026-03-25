class semaphore {
    char resource_name [64]; // the name of the resource being managed
    int sema_value; // 0 or 1 in the case of a binary semaphore
    queue *sema_queue;

    down(); // get the resource or get queued!
    up() // release the resource
    dump(int level); // include some functions which will allow you to dump the contents of the semaphore in a readable format. See the expected output section (below) for suggestions.
};