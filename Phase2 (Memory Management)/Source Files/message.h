#pragma once
#include <ctime>
#include <cstring>

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