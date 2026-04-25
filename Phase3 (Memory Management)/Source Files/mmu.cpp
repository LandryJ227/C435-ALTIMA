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

int mmu::Mem_Left() {
    int freeBlocks = 0;
    block* current = block0;

    while (current != nullptr) {
        if (current->is_free) freeBlocks++;
        current = current->nextBlock;
    }
    int totalMem = freeBlocks * BLOCK_SIZE;

    
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
    int smallest = 8; // assume smallest memory segment is the whole memory
    int free = 0;
    block* current = block0;

    // Search for a free blocks if a free block is found add it to the counter
    while (current != nullptr) {
        if (current->is_free) {
            free++;
        }
    // If an allocated block is found, check if the amount of free blocks found is less than the current smallest
    // If it is smaller, reallocate the smallest integer to the amount of free blocks found
        else {
            if (free > 0 && free < smallest) smallest = free;
            free = 0;
        }
    // Go to the next block
        current = current->nextBlock;
        
    }
    // Additional check at the end of memory search
    if (free > 0 && free < smallest) smallest = free;

    // If the smallest hole is still 8, 
    if (smallest == 8 && Mem_Left() == 0) { 
        cout << "There are no free blocks available." << endl;
        return 0; // no free blocks
    }

    int sizeSmallest = smallest * BLOCK_SIZE;
    cout << "The smallest available memory segment is " << sizeSmallest << " bytes" << endl;
    return smallest;

}


int mmu::Mem_Dump(int starting_from, int num_bytes) {
    if (num_bytes + starting_from > mmu::RAM_SIZE ) { //bytes overflow => Adjust num_bytes to runnable.
        return -1;
    }
    for (int i = starting_from; i < starting_from + num_bytes; i++) {
        if (i % 20 == 0 && i != 0 ) { //every 20 prints -> new line. can change to whatever.
            cout << mmu::mainMem[i] << endl;
        }
        else {
            cout << mmu::mainMem[i];
        }
    }
    return 0;
}

int mmu::Mem_Read(int memory_handle, char* ch) {
    block* current_block = block0;

    while (current_block != nullptr) {
        if (current_block->handle == memory_handle) {
            int block_position = current_block->current_position;
            int mem_index = current_block->start_address + block_position;

            if (current_block->is_free) return -1; //Don't Read From Free Block

            if ((block_position < mmu::BLOCK_SIZE && block_position >= 0) &&//In block bounds
                (mem_index < mmu::RAM_SIZE && mem_index >= 0)){ //In mem bounds
                    *ch = mainMem[mem_index];
                    current_block->current_position++;
                    return 0;
            }
            else return -1;
        }
        current_block = current_block->nextBlock;
    }
    return -1;
}

int mmu::Mem_Write(int memory_handle, char ch) {
    block* current_block = block0;

    while (current_block != nullptr) {
        if (current_block->handle == memory_handle) {
            int block_position = current_block->current_position;
            int mem_index = current_block->start_address + block_position;

            if (current_block->is_free) return -1; //Don't write to Free Block!

            if ((block_position < mmu::BLOCK_SIZE && block_position >= 0) &&//In block bounds
                (mem_index < mmu::RAM_SIZE && mem_index >= 0)){ //In mem bounds
                    mainMem[mem_index] = ch;
                    current_block->current_position++;
                    return 0;
                }
            else return -1;
        }
        current_block = current_block->nextBlock;
    }
    return -1;
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
    cout << "There are currently " << memleft << " bytes available." << endl;
    int handle4 = memory.Mem_Alloc(100,4);
    int handle5 = memory.Mem_Alloc(100,5);
    int memleft2 = memory.Mem_Left();
    memory.Mem_Dump(0, 129);
    small = memory.Mem_Smallest();

    cout<<endl << "reading" <<endl;
    char read;
    char* read_ptr = &read;
    memory.Mem_Read(handle1, read_ptr);
    cout << read << endl;

    cout<<"freeing"<<endl;
    int free_success = memory.Mem_Free(handle1);
    int memleft3 = memory.Mem_Left();
    memory.Mem_Dump(0, 129);
    cout<<endl << "reading" <<endl;

    memory.Mem_Read(handle1, read_ptr);
    cout << read << endl;
    cout<<"bye"<<endl;
    return 1;
}