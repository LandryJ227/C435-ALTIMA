#ifndef ALTIMA_MMU_H
#define ALTIMA_MMU_H

class mmu {
public:
    static const int BLOCK_SIZE = 128;
    char mainMem[1024];

    class block {
    public:
        const int SIZE = BLOCK_SIZE;
        int handle;
        int start_address;
        int current_position = 0;
        bool is_free = true;
        int task_id = -1;
        block* nextBlock = nullptr;

        block(int hand, int startAddr, block* next) {
            handle = hand;
            start_address = startAddr;
            nextBlock = next;
        }
    };

    block* block0;
    block* block1;
    block* block2;
    block* block3;
    block* block4;
    block* block5;
    block* block6;
    block* block7;


    mmu(int size, char default_initial_value, int page_size);//DONE
    //~mmu();//NOT DONE
    int Mem_Alloc(int size);//NOT DONE
    int Mem_Free(int memory_handle);//NOT DONE
    int Mem_Read(int memory_handle, char* ch);//NOT DONE
    int Mem_Write(int memory_handle, char ch);//NOT DONE
    int Mem_Read(int memory_handle, int offset_from_beg, int text_size, char *text);//NOT DONE
    int Mem_Write(int memory_handle, int offset_from_beg, int text_size, char *text);//NOT DONE
    int Mem_Left();//NOT DONE
    int Mem_Largest();//NOT DONE
    int Mem_Smallest();//NOT DONE
    int Mem_Coalesce();//NOT DONE
    int Mem_Dump(int starting_from, int num_bytes);//NOT DONE
};


#endif //ALTIMA_MMU_H