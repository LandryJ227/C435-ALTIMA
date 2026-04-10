#pragma once
#include <time.h>
#include "MCB.h"
#include "scheduler.h"
#include "IPC.h"


IPC::IPC(int max_tasks, MCB* mainMCB) { // Julio
    mcb = mainMCB;
    if (max_tasks <= 0) {
        // Handle error: max_tasks must be greater than 0
    }
   else {

   }
};

int IPC::Message_Send(Message *Message){ // Julio
}

int IPC::Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type) { // Julio

}

int IPC::Message_Count(int Task_Id){ // Ryan
}
int IPC::Message_Count() {// Ryan

}
int IPC::Message_DeleteAll(int Task_Id) { // Julio


}

//###################################################################################################
int IPC::Message_Receive(int Task_Id, Message *message) {
    tcb* ptrTCB = mcb->sched.process_table;//start at head of process table
    while (ptrTCB->task_id != Task_Id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTC->next;
    }
    if (ptrTCB->task_id != Task_Id) return -1;

    ptrTCB->taskMailbox.taskSema.down();                      //check the mailbox semaphore
    message = ptrTCB->taskMailbox.messageQueue.dequeue();     //grab message from queue
    ptrTCB->taskMailbox.taskSema.up();                        //free up the semaphore
    if (ptrTCB->taskMailbox.messageQueue.isEmpty()) return 0; //return 0 if no more messages
    else return 1;                                           //still messages left in mailbox
}
//###################################################################################################
int IPC::Message_Receive(int Task_Id, char *Mess, int *Mess_Type) {
    tcb* ptrTCB = mcb->sched.process_table;//start at head of process table
    while (ptrTCB->task_id != Task_Id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }
    if (ptrTCB->task_id != Task_Id) return -1;

    ptrTCB->taskMailbox.taskSema.down();                                  //check the mailbox semaphore
    Message tempMessage = ptrTCB->taskMailbox.messageQueue.dequeue();     //grab message from queue
    ptrTCB->taskMailbox.taskSema.up();                                    //free up the semaphore
    Mess = tempMessage.Msg_Text;                                         //load values into ptr parameters
    Mess_Type = tempMessage.Msg_Type.Message_Type_Id;
    if (ptrTCB->taskMailbox.messageQueue.isEmpty()) return 0;             //return 0 if no more messages
    else return 1;                                                       //still messages left in mailbox
}
//###################################################################################################
void IPC::Message_Print(int Task_id) {
    tcb* ptrTCB = mcb->sched.process_table;//start at head of process table
    while (ptrTCB->task_id != Task_Id && ptrTCB->next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB->next;
    }
    if (ptrTCB->task_id != Task_Id) return -1;

    //print all messages from queue
}
//###################################################################################################
void IPC::ipc_Message_Dump() {

}
//###################################################################################################