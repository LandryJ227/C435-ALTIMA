#include "mmu.h"
#include <iostream>
using namespace std;
class mmu;
class block;

mmu::mmu(int size, char default_initial_value, int page_size, semaphore* memSem, scheduler* s) {
    
    memorySema = memSem;
    sched = s;

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

    memorySema->down(task_id, win, dumpWin);

    block* blockPTR = block0;
    block* startBlock = nullptr;
    int contiguous = 0;

    while (blockPTR != nullptr) {
        if (blockPTR->is_free) {
            if (contiguous == 0)
                startBlock = blockPTR;
            contiguous++;

            if (contiguous == numOfBlocksNeeded) {
                block* alloc = startBlock;
                for (int i = 0; i < numOfBlocksNeeded; i++) {
                    alloc->is_free = false;
                    alloc->task_id = task_id;
                    alloc->current_position = 0;
                    alloc = alloc->nextBlock;
                }

                int handle = startBlock->handle;

                
                memorySema->up(win, dumpWin);
                return handle;
            }
        }
        else {
            startBlock = nullptr;
            contiguous = 0;
        }
        blockPTR = blockPTR->nextBlock
    }

    memorySema->up(win, dumpWin);
    return -1;
}

int mmu::Mem_Free(int memory_handle) {
    int block_start;
    block* blockPTR = block0;

    while (blockPTR != nullptr){
        if (blockPTR->handle == memory_handle) {
            block_start = blockPTR->start_address;
            if (blockPTR->is_free) {
                return -1;
            }
            blockPTR->is_free = true;
            blockPTR->task_id = -1;
            for (int i = block_start; i-block_start < blockPTR->SIZE && i<mmu::RAM_SIZE; i++) {
                mmu::mainMem[i] = '#';
            }
            return 0;
        }
        blockPTR = blockPTR->nextBlock;
    }
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