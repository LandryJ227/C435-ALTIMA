#pragma once
#ifndef C435_ALTIMA_IPC_H
#define C435_ALTIMA_IPC_H
#include <time.h>
#include "MCB.h"
#include "message_queue.h"
#include "sema.h"
#include "message.h"




class IPC {
    MCB* mcb = nullptr;

public:
/*  --------------------------------------------------------
    Message_Type
      0 = Text        — Text message, no action required
      1 = Service     — task requests a service; send result back
      2 = Notification — service completed; no action required
    --------------------------------------------------------


    */

    int ipc_init(int max_tasks, MCB* mainMCB);

    int Message_Send(Message *Message);   // Julio

    int Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type); // Julio

    int Message_Receive(int Task_Id, Message *Message); // Jacob

    int Message_Receive(int Task_Id, char *Mess, int *Mess_Type); // Jacob

    int Message_Count(int Task_Id); // Ryan

    int Message_Count(); // Ryan

    void Message_Print(int Task_Id); // Jacob

    int Message_DeleteAll(int Task_Id); // Julio

    void ipc_Message_Dump();

    ~IPC();

private:
    int max_tasks;
    int ipc_status;
    message_queue* mailbox;
    semaphore** mailbox_sem;
    MCB* mainMCB;


    bool valid_task_id(int id) const { 
        return id >= 0 && id < max_tasks;
    }

};
#endif //C435_ALTIMA_IPC_H
