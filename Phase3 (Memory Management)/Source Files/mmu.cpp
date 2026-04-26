#include "mmu.h"
#include <iostream>
using namespace std;
class block;

mmu::mmu(int size, char default_initial_value, int page_size, semaphore* memSem, scheduler* s) {
    
    // Authors: Julio and Jacob

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
    // Author: Julio

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
                    alloc->handle = startBlock->handle;
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
    // Author: Ryan
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
    // Author: Julio

    int freeBlocks = 0;
    block* current = block0;    // Block pointer to traverse memory blocks from the beginning

    // Search for a free block. If a free block is found add it to the counter,
    while (current != nullptr) {
        if (current->is_free) freeBlocks++;
        current = current->nextBlock;
    }
    
    // Multiply amount of free blocks found by the block size to convert to byte size
    int totalMem = freeBlocks * BLOCK_SIZE;

    
    return totalMem;
}

int mmu::Mem_Largest() {
    // Author: Julio

    int largest = 0;
    int free = 0;
    block* current = block0;    // Block pointer to traverse memory blocks from the beginning


    // Search for a free block. If a free block is found add it to the counter, 
    while (current != nullptr) {
        if (current->is_free) {
            free++;
            if (free > largest) largest = free;
        }
    // If allocated block is found, reset the current free block counter to 0 and keep searching for next free blocks
        else {
            free = 0;
        }
    // Go to the next block
        current = current->nextBlock;
        
    }

    // Convert block count to byte size and print result
    int sizeLargest = largest * BLOCK_SIZE;
    cout << "The largest available memory segment is " << sizeLargest << " bytes" << endl;
    return largest;

}

int mmu::Mem_Smallest() {
    // Author: Julio

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
    // Author: Ryan
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
    // Author: Ryan

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
    // Author: Ryan
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

int mmu::Mem_Read(int memory_handle, int offset_from_beg, int text_size, char *text) {
    // Author: Ryan
    if (text_size < 0 || text == nullptr) return -1;

    block* current_block = block0;

    while (current_block != nullptr) {//Search for collect block
        if (current_block->handle == memory_handle) { //In the correct block
            if (current_block->is_free) return -1; //Don't read to Free Block
            block* start_block = current_block; //tracking beginning block
            block* travel_block = start_block->nextBlock; //searching for same handle blocks
            int block_set_size = 1;

            while (travel_block != nullptr && start_block->handle == travel_block->handle) {
                if (travel_block->is_free) return -1;
                block_set_size++;
                travel_block = travel_block->nextBlock;
            }

            int block_lb = offset_from_beg;
            int block_ub = offset_from_beg + text_size; //we will read only the text_size amount.
            int mem_lb = block_lb + start_block->start_address;
            int mem_ub = block_ub + start_block->start_address;

            if ((block_ub <= mmu::BLOCK_SIZE * block_set_size && block_lb >= 0)
                && (mem_ub <= mmu::RAM_SIZE && mem_lb >= 0)) {
                for (int i = 0; i < text_size; i++) {
                    text[i] = mainMem[mem_lb + i];
                }
                return 0;
            }
            else return -1;
        }
        current_block = current_block->nextBlock;
    }
    return -1;
}

int mmu::Mem_Write(int memory_handle, int offset_from_beg, int text_size, char *text) {
    // Author: Ryan
    if (text_size < 0 || text == nullptr) return -1;

    block* current_block = block0;

    while (current_block != nullptr) {//Search for collect block
        if (current_block->handle == memory_handle) { //In the correct block
            if (current_block->is_free) return -1; //Don't write to Free Block
            block* start_block = current_block; //tracking beginning block
            block* travel_block = start_block->nextBlock; //searching for same handle blocks
            int block_set_size = 1;

            while (travel_block != nullptr && start_block->handle == travel_block->handle) {
                if (travel_block->is_free) return -1;
                block_set_size++;
                travel_block = travel_block->nextBlock;
            }

            int block_lb = offset_from_beg;
            int block_ub = offset_from_beg + text_size; //we will write only the text_size amount.
            int mem_lb = block_lb + start_block->start_address;
            int mem_ub = block_ub + start_block->start_address;



            if ((block_ub <= mmu::BLOCK_SIZE * block_set_size && block_lb >= 0)
                && (mem_ub <= mmu::RAM_SIZE && mem_lb >= 0)) {
                for (int i = 0; i < text_size; i++) {
                    mainMem[mem_lb + i] = text[i];
                }
                return 0;
                }
            else return -1;
        }
        current_block = current_block->nextBlock;
    }
    return -1;
}
int mmu::Mem_Coalesce() { //Since we used fixed block sizes coalesce loses the combining.
        // Author: Ryan
    block* current_block = block0;
    bool found = false;
    while (current_block != nullptr) {
        if (current_block->is_free) {
            int block_index = current_block->start_address;
            found = true;
            for (int i = block_index; i < mmu::BLOCK_SIZE + block_index && i < mmu::RAM_SIZE; i++) {
                mainMem[i]  = '.';
            }
        }
        current_block = current_block->nextBlock;
    }
    if (!found) return -1;
    return 0;
}

int main() {
        // Authors: Julio, Ryan, and Jacob

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


    memory.Mem_Coalesce();

    //### Writing to Block 1
    cout << "Writing to current positon in handle 4" << endl;
    const int text_size = 13;
    char text[text_size] = "Hello ultima";
    char writer;
    char *p = &writer;
    if (memory.Mem_Write(handle1, 200, text_size,text) ==-1) cout<<"Write error" <<endl;
    writer = 'h';
    memory.Mem_Write(handle1, *p);
    writer = 'e';
    if (memory.Mem_Write(handle1, *p)==-1) cout<<"error"<<endl;
    writer = 'l';
    memory.Mem_Write(handle1, *p);
    writer = 'l';
    memory.Mem_Write(handle1, *p);
    writer = 'o';
    memory.Mem_Write(handle1, *p);
    cout << "Reading from handle 5" << endl;
    char text_big[400];
    char text_new[128];
    if (memory.Mem_Read(handle1, 0, 350, text_big)==-1) {
        cout<<"Read error !!" << endl;
    }
    else {
        cout<<text_big<<endl;
    }
    memory.Mem_Dump(0,1023);

    //### Freeing block 1
    cout<<"freeing"<<endl;
    int free_success = memory.Mem_Free(handle1);
    int memleft3 = memory.Mem_Left();
    memory.Mem_Dump(0, 129);
    cout<<endl << "reading" <<endl;




    cout<<"bye"<<endl;
    return 1;
}