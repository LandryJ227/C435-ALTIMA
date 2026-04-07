#include <time.h>
#include "MCB.h"
#include "scheduler.h"

class ipc {
    MCB mcb = nulltpr;
    void connectMCB(MCB* mcb);

    struct Message_Type {
        int Message_Type_Id;
        char Message_Type_Description[64];
        
    };

    struct Message {
        int Source_Task_Id;
        int Destination_Task_Id;
        time_t Message_Arrival_Time;
        Message_Type Msg_Type;
        int Msg_Size;
        char *Msg_Text;

    };

    ipc::ipc(int max_tasks) { // Julio
        if (max_tasks <= 0) {
            // Handle error: max_tasks must be greater than 0

        }
       else {

       }
    };

    int Message_Send(Message *Message);   // Julio
    
    int Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type); // Julio

    int Message_Receive(int Task_Id, Message *Message); // Jacob

    int Message_Receive(int Task_Id, char *Mess, int *Mess_Type); // Jacob

    int Message_Count(int Task_Id); // Ryan

    int Message_Count(); // Ryan

    void Message_Print(int Task_Id); // Jacob

    int Message_DeleteAll(int Task_Id); // Julio

    void ipc_Message_Dump(); // Jacob

};

int Message_Receive(int Task_Id, Message *message) {
    tcb ptrTCB = mcb->sched.process_table;//start at head of process table
    while (ptrTCB.task_id != Task_Id && ptr.TCB.next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB.next;
    }
    if (ptrTCB.task_id != Task_Id) return -1;

    ptrTCB.taskMailbox.taskSema.down();                      //check the mailbox semaphore
    message = ptrTCB.taskMailbox.messageQueue.dequeue();     //grab message from queue
    ptrTCB.taskMailbox.taskSema.up();                        //free up the semaphore
    if (ptrTCB.taskMailbox.messageQueue.isEmpty()) return 0; //return 0 if no more messages
    else return 1;                                           //still messages left in mailbox
}
int Message_Receive(int Task_Id, char *Mess, int *Mess_Type) {
    tcb ptrTCB = mcb->sched.process_table;//start at head of process table
    while (ptrTCB.task_id != Task_Id && ptr.TCB.next != NULL) {//search for correct tcb with task id
        ptrTCB = ptrTCB.next;
    }
    if (ptrTCB.task_id != Task_Id) return -1;

    ptrTCB.taskMailbox.taskSema.down();                                  //check the mailbox semaphore
    Message tempMessage = ptrTCB.taskMailbox.messageQueue.dequeue();     //grab message from queue
    ptrTCB.taskMailbox.taskSema.up();                                    //free up the semaphore
    Mess = tempMessage.Msg_Text;                                         //load values into ptr parameters
    Mess_Type = tempMessage.Msg_Type.Message_Type_Id;
    if (ptrTCB.taskMailbox.messageQueue.isEmpty()) return 0;             //return 0 if no more messages
    else return 1;                                                       //still messages left in mailbox
}
void connectMCB(MCB* mainMCB) {
    mcb = mainMCB;
}