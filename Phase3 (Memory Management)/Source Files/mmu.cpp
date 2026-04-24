#include "mmu.h"
#include <iostream>
using namespace std;
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

int mmu::Mem_Alloc(int size, int task_id) {
    int numOfBlocksNeeded = size / BLOCK_SIZE;
    if ((size % 128) != 0) numOfBlocksNeeded++;

    //memorySema->down(task_id, win, dumpWin);

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
                cout << "Successfully allocated to Block " << handle << endl;

                
                //memorySema->up(win, dumpWin);
                return handle;
            }
        }

        // not enough contiguous blocks, reset counter and keep searching
        else {
            startBlock = nullptr;
            contiguous = 0;
        }

        blockPTR = blockPTR->nextBlock;
    }

    //memorySema->up(win, dumpWin);
    cerr << "ERROR. NOT ENOUGH RAM AVAILABLE." << endl;
    return -1;
}

int mmu::Mem_Left() {
    int freeBlocks = 0;
    block* current = block0;

    while (current != nullptr) {
        if (current->is_free) freeBlocks++;
        current = current->nextBlock;
    }
    int totalMem = freeBlocks * BLOCK_SIZE;

    cout << "There are currently " << totalMem << " bytes of memory remaining." << endl;
    return totalMem;
}

int mmu::Mem_Largest() {
    int largest = 0;
    int free = 0;
    block* current = block0;

    while (current != nullptr) {
        if (current->is_free) {
            free++;
            if (free > largest) largest = free;
        }
        else {
            free = 0;
        }
        current = current->nextBlock;
        
    }
    int sizeLargest = largest * BLOCK_SIZE;
    cout << "The largest available memory segment is " << sizeLargest << " bytes" << endl;
    return largest;

}

int mmu::Mem_Smallest() {
    int smallest = 0;
    int free = 0;
    block* current = block0;

    while (current != nullptr) {
        if (current->is_free) {
            free++;
        }
        else {
            if (free > 0 && free < smallest) smallest = free;
            free = 0;
        }
        current = current->nextBlock;
        
    }
    if (free > 0 && free < smallest) smallest = free;
    if (smallest == 8) { 
        cout << "There are no free blocks available." << endl;
        return 0; // no free blocks
    }

    int sizeSmallest = smallest * BLOCK_SIZE;
    cout << "The smallest available memory segment is " << sizeSmallest << " bytes" << endl;
    return smallest;

}


int main() {
    semaphore* memorySema;
    scheduler* sched;
    mmu memory(1024, '.', 128, memorySema, sched);
    //cout << memory.block3->handle << endl;
    //cout << memory.block3->start_address << endl;
    //cout << memory.block3->current_position << endl;
    //cout << memory.block3->is_free << endl;
    //out << memory.block3->task_id << endl;
    int large = memory.Mem_Largest();
    int small = memory.Mem_Smallest();
    int handle1 = memory.Mem_Alloc(400,1);
    large = memory.Mem_Largest();
    int handle2 = memory.Mem_Alloc(200,2);
    small = memory.Mem_Smallest();
    int handle3 = memory.Mem_Alloc(800,3);
    int memleft = memory.Mem_Left();
    int handle4 = memory.Mem_Alloc(100,4);
    int handle5 = memory.Mem_Alloc(100,5);
    int memleft2 = memory.Mem_Left();

    return 1;
}