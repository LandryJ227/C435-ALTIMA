#include <time.h>
#include "MCB.h"
#include "scheduler.h"
#include "IPC.h"
#include "Ultima.h"
/*
int IPC::ipc_init(int max_tasks, MCB* mainMCB) { // Julio

    // Handle error: max_tasks must be greater than 0

    if (max_tasks <= 0 || mainMCB == nullptr) {
        std::cerr << "[IPC] ipc_init failed: Invalid arguments.\n";
        ipc_status = -1;
        return -1;

    }

    this->max_tasks = max_tasks;
    this->mainMCB = mainMCB;

    // Allocate memory for mailboxes
    mailboxes = new (std::nothrow) std::queue<Message>[max_tasks];
    if (!mailboxes) {
        std::cerr << "[IPC] ipc_init failed: Memory allocation for mailboxes failed. \n";
        ipc_status = -1;
        return -1;

    }

    // Allocate memory for mailbox semaphores
    mailbox_sem = new (std::nothrow) semaphore*[max_tasks];
    if (!mailbox_sem) {
        std::cerr << "[IPC] ipc_init failed: Memory allocation for mailbox semaphores failed.\n";
        delete[] mailboxes; // Clean up previously allocated mailboxes
        ipc_status = -1;
        return -1;
    }

    // Construct semaphores for each mailbox
    // TODO



    std::cout << " [IPC] Initialized with " << max_tasks << " mailboxes.\n";
    ipc_status = 1; // Success
    return 1;
};
*/
int IPC::Message_Send(Message *message, WINDOW * win, WINDOW* dumpWin){ // Julio
    write_window(win, 25,1, "A");
// #################    ERROR CHECKING BEFORE COPYING MESSAGE OR ENQUEUING  ###########################

    if (!message) {
        cerr << "[IPC] Message_Send: null message pointer.\n";
        return -1;
    }
    // Find destination task from TCB
    tcb* destTCB = mcb->sched->process_table;
    while(destTCB != nullptr && destTCB->task_id != message->Destination_Task_Id) {
        destTCB = destTCB->next;
    }

    if (destTCB == nullptr) {
        snprintf(tempStr, sizeof(tempStr), "[IPC] Message_Send: Destination task id %d not found.\n", message->Destination_Task_Id);
        return -1;
    }

    // Find source task from TCB
    tcb* sourceTCB = mcb->sched->process_table;
    while (sourceTCB != nullptr && sourceTCB->task_id != message->Source_Task_Id) {
        sourceTCB = sourceTCB->next;
    }

    if (sourceTCB == nullptr) {
        snprintf(tempStr, sizeof(tempStr), "[IPC] Message_Send: Source task id %d not found.\n", message->Source_Task_Id);
        return -1;
    }

    // Check message size
    if (message->Msg_Size < 0 || message->Msg_Size > 31) {
        snprintf(tempStr, sizeof(tempStr), "[IPC] Message_Send: Invalid message size %d. Must be between 0 and 31.\n", message->Msg_Size);
        return -1;
    }

    // Check if destination mailbox is full
    if (destTCB->taskMailbox.messageQueue->size >= message_queue::QUEUE_SIZE) {
        snprintf(tempStr, sizeof(tempStr), "[IPC] Message_Send: Destination mailbox for task %d is full.\n", message->Destination_Task_Id);
        return -1;
    }
//####################### BUILD CLEAN COPY OF MESSAGE ############################################
    Message msg_copy; // Create a copy of the message to enqueue

    msg_copy.Source_Task_Id = message->Source_Task_Id;
    msg_copy.Destination_Task_Id = message->Destination_Task_Id;
    msg_copy.Message_Arrival_Time = time(nullptr); // Set arrival time to current time
    msg_copy.Msg_Type = message->Msg_Type;
    msg_copy.Msg_Size = message->Msg_Size;
    msg_copy.Msg_Text = new char[32];
    strncpy(msg_copy.Msg_Text, message->Msg_Text, 31);
    msg_copy.Msg_Text[31] = '\0';

    destTCB->taskMailbox.mailSema->down(message->Source_Task_Id, win, dumpWin);
    destTCB->taskMailbox.messageQueue->enqueue(msg_copy);
    destTCB->taskMailbox.mailSema->up(win, dumpWin);


    snprintf(tempStr, sizeof(tempStr),
             "[IPC] Sent: Task %d -> Task %d | Type: %d | \"%s\"",
             msg_copy.Source_Task_Id,
             msg_copy.Destination_Task_Id,
             msg_copy.Msg_Type.Message_Type_Id,
             msg_copy.Msg_Text);

    return 1;
}
/*
int IPC::Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type) { // Julio

    // TODO
return 1;
}

int IPC::Message_Count(int Task_Id){ // Ryan
    return 1;
}
int IPC::Message_Count() {// Ryan
    return 1;

}
int IPC::Message_DeleteAll(int Task_Id) { // Julio
    return 1;


    // TODO

} */
//###################################################################################################
int IPC::Message_Receive(int Task_Id, Message *message, WINDOW* semaWin) {
    tcb* ptrTCB = mcb->sched->process_table;//start at head of process table
    while (ptrTCB->task_id != Task_Id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }
    if (ptrTCB->task_id != Task_Id) return -1;

    ptrTCB->taskMailbox.mailSema->down(Task_Id, IPCwin, semaWin);                      //check the mailbox semaphore
    *message = ptrTCB->taskMailbox.messageQueue->dequeue();     //grab message from queue
    ptrTCB->taskMailbox.mailSema->up(IPCwin, semaWin);                        //free up the semaphore
    if (ptrTCB->taskMailbox.messageQueue->isEmpty()) return 0; //return 0 if no more messages
    else return 1;                                           //still messages left in mailbox
}
//###################################################################################################
int IPC::Message_Receive(int Task_Id, char *Mess, int *Mess_Type, WINDOW* semaWin) {
    tcb* ptrTCB = mcb->sched->process_table;//start at head of process table
    while (ptrTCB->task_id != Task_Id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }
    if (ptrTCB->task_id != Task_Id) return -1;

    ptrTCB->taskMailbox.mailSema->down(Task_Id, IPCwin, semaWin);//check the mailbox semaphore
    Message tempMessage = ptrTCB->taskMailbox.messageQueue->dequeue();     //grab message from queue
    ptrTCB->taskMailbox.mailSema->up(IPCwin, semaWin);         //free up the semaphore
    Mess = tempMessage.Msg_Text;                                         //load values into ptr parameters
    *Mess_Type = tempMessage.Msg_Type.Message_Type_Id;
    if (ptrTCB->taskMailbox.messageQueue->isEmpty()) return 0;             //return 0 if no more messages
    else return 1;                                                       //still messages left in mailbox
}
//###################################################################################################
void IPC::Message_Print(int Task_id, WINDOW* win) {
    int outputLine = 3;
    tcb* ptrTCB = mcb->sched->process_table;//start at head of process table
    while (ptrTCB->task_id != Task_id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }
    snprintf(tempStr, sizeof(tempStr), "All messages in queue for task #%d:", Task_id);
    write_window(win, outputLine++, 2, tempStr);

    for (int i = ptrTCB->taskMailbox.messageQueue->head; i != ptrTCB->taskMailbox.messageQueue->tail; i = (i + 1) % ptrTCB->taskMailbox.messageQueue->QUEUE_SIZE) {
        string tempMessage = ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Text;
        write_window(win, outputLine++, 2, tempMessage.c_str());
    }

}
//###################################################################################################
void IPC::ipc_Message_Dump(WINDOW* win) {
    int outputLine = 3;
    tcb* ptrTCB = mcb->sched->process_table;
    while (ptrTCB != NULL) {
        snprintf(tempStr, sizeof(tempStr), "All messages in queue for task #%d:", ptrTCB->task_id);
        write_window(win, outputLine++, 2, tempStr);

        for (int i = ptrTCB->taskMailbox.messageQueue->head; i != ptrTCB->taskMailbox.messageQueue->tail; i = (i + 1) % ptrTCB->taskMailbox.messageQueue->QUEUE_SIZE) {
            string tempMessage = ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Text;
            write_window(win, outputLine++, 2, tempMessage.c_str());
        }
    }
}
//###################################################################################################

//IPC::~IPC() {

//}

void IPC::setMCB(MCB* mainMCB) {
    mcb = mainMCB;
}