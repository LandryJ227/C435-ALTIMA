#pragma once
#ifndef C435_ALTIMA_IPC_H
#define C435_ALTIMA_IPC_H
#include <time.h>
#include "MCB.h"
#include "scheduler.h"


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
    struct Message_Type {
        int Message_Type_Id;
        char Message_Type_Description[64];


        // Default constructor (Message_Type mt;)
        //     -Initializes the Message_Type_Id to 0 and Message_Type_Description to an empty string

        Message_Type() : Message_Type_Id(0) {
            memset(Message_Type_Description, 0, sizeof(Message_Type_Description));

        }


        // Parameterized constructor (Message_Type mt(1, "Service");)
        //      -Initializes the Message_Type_Id with provided id

        Message_Type(int id, const char* desc) : Message_Type_Id(id) {
            strncpy(Message_Type_Description, desc, 63);
            Message_Type_Description[63] = '\0';    // Ensure null-termination

        }

    };

    struct Message {
        int Source_Task_Id;
        int Destination_Task_Id;
        time_t Message_Arrival_Time;
        Message_Type Msg_Type;
        int Msg_Size;
        char *Msg_Text;

        // Default constructor (Message msg;)
        //     -Initializes all members to their default values
        Message() : Source_Task_Id(-1),         // no task yet
                    Destination_Task_Id(-1),    // unset, no task
                    Message_Arrival_Time(0),    // no time yet
                    Msg_Size(0){                // empty message
                    memset(Msg_Text, 0, sizeof(Msg_Text));
                }

    };

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
    std::queue<Message>* mailbox;
    semaphore** mailbox_sem;
    MCB* mainMCB;


    bool valid_task_id(int id) const { 
        return id >= 0 && id < max_tasks;
    }

};
#endif //C435_ALTIMA_IPC_H
