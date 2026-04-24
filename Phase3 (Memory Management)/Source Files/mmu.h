#ifndef ALTIMA_MMU_H
#define ALTIMA_MMU_H
#include ""


class mmu {
    class block {
        int mem_handle = -1;
        int taskAllocatedTo = -1;

        int starting_from = 0;
        int num_bytes = 0;

        bool block_free = true;
        block* next = nullptr;
    };

    block mainMem[16];


    mmu(int size, char default_initial_value, int page_size);
    ~mmu();
    int Mem_Alloc(int size);
    int Mem_Free(int memory_handle);
    int Mem_Read(int memory_handle, char* ch);
    int Mem_Write(int memory_handle, char ch);
    int Mem_Read(int memory_handle, int offset_from_beg, int text_size, char *text);
    int Mem_Write(int memory_handle, int offset_from_beg, int text_size, char *text);
    int Mem_Left();
    int Mem_Largest();
    int Mem_Smallest();
    int Mem_Coalesce();
    int Mem_Dump(int starting_from, int num_bytes);
};


#endif //ALTIMA_MMU_H