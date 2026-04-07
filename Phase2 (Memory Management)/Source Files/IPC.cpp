#include <time.h>

class ipc {
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

    int ipc(int max_tasks) {
        
    };

    int Message_Send(.,Message *Message);
    
    int Message_Send(int S_Id, int D_Id, char *Mess, int Mess_Type);

    int Message_Receive(int Task_Id, Message *Message);

    int Message_Receive(int Task_Id, char *Mess, int *Mess_Type);

    int Message_Count(int Task_Id);

    int Message_Count();

    void Message_Print(int Task_Id);

    int Message_DeleteAll(int Task_Id);

    void ipc_Message_Dump();

}