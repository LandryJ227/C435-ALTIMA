#include "mmu.h"
#include <iostream>
#include "Ultima.h"
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

int mmu::Mem_Alloc(int size, int task_id, WINDOW* win, WINDOW* memWin) {
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

                int allocWriteLine = (handle*6)+2;
                snprintf(buf, sizeof(buf), "Allocated to task %d", task_id);
                write_window(memWin, allocWriteLine, 21, buf);

                snprintf(buf, sizeof(buf), "Successfully allocated to Block %d", handle);
                write_window(win, memWriteLine++, 1, buf);
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
    //cerr << "ERROR. NOT ENOUGH RAM AVAILABLE." << endl;
    return -1;
}

int mmu::Mem_Free(int memory_handle, WINDOW* win) {
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
                updateMemoryWin(win, i, '#');
            }
            //return 0;
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
    //cout << "The largest available memory segment is " << sizeLargest << " bytes" << endl;
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
        //cout << "There are no free blocks available." << endl;
        return 0; // no free blocks
    }

    int sizeSmallest = smallest * BLOCK_SIZE;
    //cout << "The smallest available memory segment is " << sizeSmallest << " bytes" << endl;
    return smallest;

}


int mmu::Mem_Dump(int starting_from, int num_bytes) {
    // Author: Ryan
    if (num_bytes + starting_from > mmu::RAM_SIZE ) { //bytes overflow => Adjust num_bytes to runnable.
        return -1;
    }
    for (int i = starting_from; i < starting_from + num_bytes; i++) {
        if (i % 20 == 0 && i != 0 ) { //every 20 prints -> new line. can change to whatever.
            //cout << mmu::mainMem[i] << endl;
        }
        else {
            //cout << mmu::mainMem[i];
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

int mmu::Mem_Write(int memory_handle, char ch, WINDOW* win) {
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
                    updateMemoryWin(win, mem_index, ch);
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

int mmu::Mem_Write(int memory_handle, int offset_from_beg, int text_size, char *text, WINDOW* win) {
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
                    updateMemoryWin(win, mem_lb+i, text[i]);
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

void mmu::updateMemoryWin(WINDOW* win, int memPos, char data) {
    int memRow;
    int memCol;

    memRow = memPos / 32;
    memCol = memPos % 32;

    if (memRow <= 3) memRow += 3;
    else if (memRow <=7) memRow += 5;
    else if (memRow <=11) memRow += 7;
    else if (memRow <=15) memRow += 9;
    else if (memRow <=19) memRow += 11;
    else if (memRow <=23) memRow += 13;
    else if (memRow <=27) memRow += 15;
    else if (memRow <=31) memRow += 17;

    if (memCol <= 7) memCol += 2;
    else if (memCol <= 15) memCol += 5;
    else if (memCol <= 23) memCol += 8;
    else if (memCol <= 31) memCol +=11;

    char buf[2] = { (char)data, '\0' };
    write_window(win, memRow, memCol, buf);
}