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

// #################    ERROR CHECKING BEFORE COPYING MESSAGE OR ENQUEUING  ###########################
    if (ipc_status == -1) {
        cerr << "[IPC] Message_Send: IPC not initialized.\n";
        return -1;
    }
    if (!message) {
        cerr << "[IPC] Message_Send: null message pointer.\n";
        return -1;
    }
    if (!valid_task_id(message->Source_Task_Id)) {
        cerr << "[IPC] Message_Send: invalid source task id "
             << message->Source_Task_Id << ".\n";
        return -1;
    }
    if (!valid_task_id(message->Destination_Task_Id)) {
        cerr << "[IPC] Message_Send: invalid destination task id "
             << message->Destination_Task_Id << ".\n";
        return -1;
    }
    if (message->Msg_Size < 0 || message->Msg_Size > 32) {
        cerr << "[IPC] Message_Send: Msg_Size out of range (0-32).\n";
        return -1;
    }
//#################################################################################################

    Message msg_copy; // Create a copy of the message to enqueue

    msg_copy.Source_Task_Id = message->Source_Task_Id;
    msg_copy.Destination_Task_Id = message->Destination_Task_Id;
    msg_copy.Message_Arrival_Time = time(nullptr); // Set arrival time to current time
    msg_copy.Msg_Type = message->Msg_Type;
    msg_copy.Msg_Size = message->Msg_Size;
    strncpy(msg_copy.Msg_Text, message->Msg_Text, 31);
    msg_copy.Msg_Text[31] = '\0'; // Ensure null-termination)

    int destination_id = message->Destination_Task_Id;
    mailbox[destination_id].enqueue(msg_copy); // Enqueue the message to the destination's mailbox


    cout << "[IPC] Message sent: Task " << msg_copy.Source_Task_Id
         << " -> Task "  << dest
         << " | Type: "  << msg_copy.Msg_Type.Message_Type_Id
         << " | Size: "  << msg_copy.Msg_Size
         << " | Text: \"" << msg_copy.Msg_Text << "\"\n";


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

IPC::~IPC() {

}