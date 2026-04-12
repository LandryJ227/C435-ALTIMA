#pragma once
#ifndef C435_ALTIMA_IPC_H
#define C435_ALTIMA_IPC_H
#include <time.h>
#include "MCB.h"
#include "message_queue.h"
#include "sema.h"
#include "message.h"
#include "scheduler.h"
#include <cstring>

class IPC {
    MCB* mcb = nullptr;
    //WINDOW* IPCwin;

public:
/*  --------------------------------------------------------
    Message_Type
      0 = Text        — Text message, no action required
      1 = Service     — task requests a service; send result back
      2 = Notification — service completed; no action required
    --------------------------------------------------------


    */

    void setMCB(MCB* mainMCB);

    int ipc_init(int max_tasks, MCB* mainMCB);

    int Message_Send(Message *message, WINDOW * win, WINDOW* dumpWin);   // Julio

    int Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type, WINDOW* outputWin); // Julio


    int Message_Receive(int Task_Id, Message *message, WINDOW* semaWin, WINDOW* outputWin); // Jacob

    int Message_Receive(int Task_Id, char *Mess, int *Mess_Type, WINDOW* semaWin, WINDOW* outputWin); // Jacob

    int Message_Count(int Task_Id); // Ryan

    int Message_Count(); // Ryan

    void Message_Print(int Task_Id, WINDOW* win); // Jacob

    int Message_DeleteAll(int Task_Id, WINDOW* outputWin); // Julio

    void ipc_Message_Dump(WINDOW* win);

   // ~IPC();


private:
    int max_tasks;
    int ipc_status;
    //std::queue<Message>* mailbox;
    //semaphore** mailbox_sem;
    //message_queue* mailbox;
    //semaphore** mailbox_sem;
    //MCB* mainMCB;


    bool valid_task_id(int id) const { 
        return id >= 0 && id < max_tasks;
    }

};
#endif //C435_ALTIMA_IPC_H
