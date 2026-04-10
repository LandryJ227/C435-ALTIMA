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
int IPC::Message_Send(Message *Message){ // Julio

    // TODO

    return 1;
}

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

}

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