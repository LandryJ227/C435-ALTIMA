#include <time.h>
#include "MCB.h"
#include "scheduler.h"
#include "IPC.h"
#include "Ultima.h"

int IPC::ipc_init(int max_tasks, MCB* mainMCB) {
    // Julio

    // Handle error: max_tasks must be greater than 0
    if (max_tasks <= 0 || mainMCB == nullptr){
        ipc_status = -1; // Set error status
        return -1; 
    }

    this->max_tasks = max_tasks;
    this->mcb = mainMCB;

    ipc_status = 1; // Set success status
    return 1;
}
//###########################################################################################################

int IPC::Message_Send(Message *message, WINDOW * win, WINDOW* dumpWin) {
    // Julio
    // ========================    ERROR CHECKING BEFORE COPYING MESSAGE OR ENQUEUING  ==============================

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
    //===================================== END ERROR CHECKING ================================================================

    //================================= BUILD CLEAN COPY OF MESSAGE ===========================================================
    Message msg_copy; // Create a copy of the message to enqueue

    msg_copy.Source_Task_Id = message->Source_Task_Id;              // copy source task id
    msg_copy.Destination_Task_Id = message->Destination_Task_Id;    // copy destination task id
    msg_copy.Message_Arrival_Time = time(nullptr);                  // set arrival time to current time
    msg_copy.Msg_Type = message->Msg_Type;                          // copy message type
    msg_copy.Msg_Size = message->Msg_Size;                          // copy message size
    msg_copy.Msg_Text = new char[32];                               // allocate memory for message text (max 31 chars + null terminator)
    strncpy(msg_copy.Msg_Text, message->Msg_Text, 31);              // copy message text with size limit
    msg_copy.Msg_Text[31] = '\0';                                   // ensure null termination  

    destTCB->taskMailbox.mailSema->down(message->Destination_Task_Id, win, dumpWin); // Lock the destination mailbox semaphore before enqueuing
    destTCB->taskMailbox.messageQueue->enqueue(msg_copy);                       // Enqueue the message copy into the destination mailbox
    destTCB->taskMailbox.mailSema->up(win, dumpWin);                            // Unlock the destination mailbox semaphore after enqueuing  


    snprintf(tempStr, sizeof(tempStr),
             "[IPC] Sent: Task %d -> Task %d | Type: %d | \"%s\"",
             msg_copy.Source_Task_Id,
             msg_copy.Destination_Task_Id,
             msg_copy.Msg_Type.Message_Type_Id,
             msg_copy.Msg_Text);

    return 1;
}
//####################################################################################################################


    // ========================    ERROR CHECKING BEFORE COPYING MESSAGE OR ENQUEUING  ==============================
int IPC::Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type, WINDOW* outputWin) { // Julio
    if (!Mess) {
        write_window(outputWin, outputWriteLine++, 12, "[IPC] Message_Send: null message text pointer.");
        return -1;
    }

    if (Mess_Type < 0 || Mess_Type > 2) {
        write_window(outputWin, outputWriteLine++, 12, "[IPC] Message_Send: Invalid message type. Must be 0 (Text), 1 (Service), or 2 (Notification).");
        return -1;
    }
    // =========================================   END ERROR CHECKING  ================================================
    // BUILD THE MESSAGE STRUCTURE
    Message msg;
    msg.Source_Task_Id = S_Id;
    msg.Destination_Task_Id = D_Id;
    msg.Msg_Size = static_cast<int>(strnlen(Mess, 32)); // Get message size with max limit of 31 chars

    msg.Msg_Text = new char[32]; // Allocate memory for message text
    strncpy(msg.Msg_Text, Mess, 31); // Copy message text with size limit
    msg.Msg_Text[31] = '\0'; // Ensure null termination

    msg.Msg_Type.Message_Type_Id = Mess_Type;
    switch (Mess_Type) {
        case 0:
            strncpy(msg.Msg_Type.Message_Type_Description, "Text", 63);
            break;
        case 1:
            strncpy(msg.Msg_Type.Message_Type_Description, "Service", 63);
            break;
        case 2:
            strncpy(msg.Msg_Type.Message_Type_Description, "Notification", 63);
            break;
    }

    int result = Message_Send(&msg, outputWin, outputWin);
    delete[] msg.Msg_Text; // Clean up allocated memory for message text
    return result;
}
int IPC::Message_Count(int Task_Id){
    if (mcb == nullptr || mcb->sched == nullptr) return -1;

    tcb* ptrTCB = mcb->sched->process_table;
    while (ptrTCB != nullptr && ptrTCB->task_id != Task_Id) {
        ptrTCB = ptrTCB->next;
    }

    if (ptrTCB == nullptr) return -1;

    return ptrTCB->taskMailbox.messageQueue->size;
}

int IPC::Message_Count() {
    if (mcb == nullptr || mcb->sched == nullptr) return -1;

    int total = 0;
    tcb* ptrTCB = mcb->sched->process_table;

    while (ptrTCB != nullptr) {
        total += ptrTCB->taskMailbox.messageQueue->size;
        ptrTCB = ptrTCB->next;
    }

    return total;
}

int IPC::Message_DeleteAll(int Task_Id, WINDOW* outputWin) { // Julio
    // Traverse TCB list to find task
    tcb* ptrTCB = mcb->sched->process_table;
    while(ptrTCB != nullptr && ptrTCB->task_id != Task_Id) {
        ptrTCB = ptrTCB->next;
    }

    // If we reach the end of the list without finding the task, it doesn't exist
    if (ptrTCB == nullptr) {
        snprintf(tempStr, sizeof(tempStr), "[IPC] Message_DeleteAll: Task id %d not found.\n", Task_Id);
        write_window(outputWin, outputWriteLine++, 12, tempStr);
        return -1;
    }

    // If the mailbox is already empty, there's nothing to delete we return 0
    if (ptrTCB->taskMailbox.messageQueue->isEmpty()) {
        snprintf(tempStr, sizeof(tempStr), "[IPC] Message_DeleteAll: Mailbox for task %d is already empty.\n", Task_Id);
        write_window(outputWin, outputWriteLine++, 12, tempStr);
        return 0;
    }

    ptrTCB->taskMailbox.mailSema->down(Task_Id, outputWin, outputWin);
    // Acquire mailbox semaphore before modifying message queue
    // -Prevents another thread from enq/dq during delete process

    // Instantiate variables to navigate through circular message queue
    int i = ptrTCB->taskMailbox.messageQueue->head;
    int count = ptrTCB->taskMailbox.messageQueue->size;
    int qsize = ptrTCB->taskMailbox.messageQueue->QUEUE_SIZE;

    // Loop through all messages in the queue and delete the allocated text
    for (int n = 0; n < count; n++) {
        delete [] ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Text;
        ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Text = nullptr;
        i = (i+1) % qsize; 
    }

    // Reset circular queue pointers
    ptrTCB->taskMailbox.messageQueue->size = 0;
    ptrTCB->taskMailbox.messageQueue->head = 0;
    ptrTCB->taskMailbox.messageQueue->tail = 0;

    // Unlock mailbox semaphore once deletion is complete
    ptrTCB->taskMailbox.mailSema->up(outputWin, outputWin);

    // Log # of messages deleted
    snprintf(tempStr, sizeof(tempStr), "[IPC] Message_DeleteAll: Deleted %d messages from task %d mailbox.\n", count, Task_Id);
    write_window(outputWin, outputWriteLine++, 12, tempStr);

    return 1;

} 
//###################################################################################################
int IPC::Message_Receive(int Task_Id, Message *message, WINDOW* semaWin, WINDOW* outputWin) {
    tcb* ptrTCB = mcb->sched->process_table;//start at head of process table
    while (ptrTCB->task_id != Task_Id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }
    if (ptrTCB->task_id != Task_Id) return -1;

    ptrTCB->taskMailbox.mailSema->down(Task_Id, outputWin, semaWin);                      //check the mailbox semaphore

    if (ptrTCB->taskMailbox.messageQueue->isEmpty()) return -1;

    *message = ptrTCB->taskMailbox.messageQueue->dequeue();     //grab message from queue
    ptrTCB->taskMailbox.mailSema->up(outputWin, semaWin);                        //free up the semaphore
    if (ptrTCB->taskMailbox.messageQueue->isEmpty()) return 0; //return 0 if no more messages
    else return 1;                                           //still messages left in mailbox
}
//###################################################################################################
int IPC::Message_Receive(int Task_Id, char *Mess, int *Mess_Type, WINDOW* semaWin, WINDOW* outputWin) {
    tcb* ptrTCB = mcb->sched->process_table;//start at head of process table
    while (ptrTCB->task_id != Task_Id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }
    if (ptrTCB->task_id != Task_Id) return -1;

    ptrTCB->taskMailbox.mailSema->down(Task_Id, outputWin, semaWin);//check the mailbox semaphore
    Message tempMessage = ptrTCB->taskMailbox.messageQueue->dequeue();     //grab message from queue
    ptrTCB->taskMailbox.mailSema->up(outputWin, semaWin);         //free up the semaphore
    Mess = tempMessage.Msg_Text;                                         //load values into ptr parameters
    *Mess_Type = tempMessage.Msg_Type.Message_Type_Id;
    if (ptrTCB->taskMailbox.messageQueue->isEmpty()) return 0;             //return 0 if no more messages
    else return 1;                                                       //still messages left in mailbox
}
//###################################################################################################
void IPC::Message_Print(int Task_id, WINDOW* win) {
    int outputLine = 3;
    char buf[256];

    //Reset window from last print
    wclear(win);
    write_window(win, 1, 16, "--- Message Dump Win ---");

    tcb* ptrTCB = mcb->sched->process_table;                    //start at head of process table
    while (ptrTCB->task_id != Task_id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }

    //Output all correct information
    snprintf(buf, sizeof(buf), "Called by thread %d:", Task_id);
    write_window(win, outputLine++, 2, buf);
    write_window(win, outputLine++, 2, "Source-ID   Dest-ID   Message-Content               Size     Type");
    for (int i = ptrTCB->taskMailbox.messageQueue->head; i != ptrTCB->taskMailbox.messageQueue->tail; i = (i + 1) % ptrTCB->taskMailbox.messageQueue->QUEUE_SIZE) {
        snprintf(buf, sizeof(buf), "    %d          %d      %s      %d        %d",
            ptrTCB->taskMailbox.messageQueue->messageQueue[i].Source_Task_Id,
            ptrTCB->taskMailbox.messageQueue->messageQueue[i].Destination_Task_Id,
            ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Text,
            ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Size,
            ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Type.Message_Type_Id);
        write_window(win, outputLine++, 2, buf);
    }

}
//###################################################################################################
void IPC::ipc_Message_Dump(WINDOW* win) {
    int outputLine = 3;
    char buf[256];

    //Reset window from last dump
    wclear(win);
    write_window(win, 1, 16, "--- Message Dump Win ---");
    write_window(win, outputLine++, 2, "Source-ID   Dest-ID   Message-Content               Size     Type");

    //For each task, if they have any messages, print them
    tcb* ptrTCB = mcb->sched->process_table;
    while (ptrTCB != NULL) {
        for (int i = ptrTCB->taskMailbox.messageQueue->head; i != ptrTCB->taskMailbox.messageQueue->tail; i = (i + 1) % ptrTCB->taskMailbox.messageQueue->QUEUE_SIZE) {
            snprintf(buf, sizeof(buf), "    %d          %d      %s      %d        %d",
                ptrTCB->taskMailbox.messageQueue->messageQueue[i].Source_Task_Id,
                ptrTCB->taskMailbox.messageQueue->messageQueue[i].Destination_Task_Id,
                ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Text,
                ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Size,
                ptrTCB->taskMailbox.messageQueue->messageQueue[i].Msg_Type.Message_Type_Id);
            write_window(win, outputLine++, 2, buf);
        }
        ptrTCB = ptrTCB->next;
    }
}

//###################################################################################################

//IPC::~IPC() {

//}

void IPC::setMCB(MCB* mainMCB) {
    //Gives the ipc object a way to address the mcb
    mcb = mainMCB;
}