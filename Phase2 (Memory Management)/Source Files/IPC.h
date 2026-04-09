#pragma once
#ifndef C435_ALTIMA_IPC_H
#define C435_ALTIMA_IPC_H
#include <time.h>
#include "MCB.h"
#include "scheduler.h"


class IPC {
    MCB* mcb = nullptr;

public:
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

    IPC(int max_tasks, MCB* mainMCB);

    int Message_Send(Message *Message);   // Julio

    int Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type); // Julio

    int Message_Receive(int Task_Id, Message *Message); // Jacob

    int Message_Receive(int Task_Id, char *Mess, int *Mess_Type); // Jacob

    int Message_Count(int Task_Id); // Ryan

    int Message_Count(); // Ryan

    void Message_Print(int Task_Id); // Jacob

    int Message_DeleteAll(int Task_Id); // Julio

    void ipc_Message_Dump();

};
#endif //C435_ALTIMA_IPC_H
