#include "mmu.h"
#include <iostream>

using namespace std;
class mmu;
class block;

mmu::mmu(int size, char default_initial_value, int page_size) {

    for (int i=0; i<size; i++) {
        mainMem[i] = default_initial_value;
    }
    
    int currentHandle=7;
    block7 = new block(currentHandle, currentHandle * page_size, nullptr);
    currentHandle--;
    block6 = new block(currentHandle, currentHandle * page_size, block7);
    currentHandle--;
    block5 = new block(currentHandle, currentHandle * page_size, block6);
    currentHandle--;
    block4 = new block(currentHandle, currentHandle * page_size, block5);
    currentHandle--;
    block3 = new block(currentHandle, currentHandle * page_size, block4);
    currentHandle--;
    block2 = new block(currentHandle, currentHandle * page_size, block3);
    currentHandle--;
    block1 = new block(currentHandle, currentHandle * page_size, block2);
    currentHandle--;
    block0 = new block(currentHandle, currentHandle * page_size, block1);
}

int mmu::Mem_Alloc(int size) {
    int numOfBlocksNeeded = size / BLOCK_SIZE;
    if ((size % 128) != 0) numOfBlocksNeeded++;
    block* blockPTR = block0;






}


int Mem_Free(int memory_handle) {




    return -1;
}

int main() {
    mmu memory(1024, '.', 128);
    cout << memory.block3->handle << endl;
    cout << memory.block3->start_address << endl;
    cout << memory.block3->current_position << endl;
    cout << memory.block3->is_free << endl;
    cout << memory.block3->task_id << endl;
    return 1;
}