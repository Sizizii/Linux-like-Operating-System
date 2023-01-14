#include "paging.h"


// // load the page directory to find the byte
// void loadPageDirectory();

// // to enable the page with the connection 
// void Enable_paging();


/* paging_init: initialize the page directory and page table
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the paging directory and paging table
*/
void paging_init(void)
{
    int index;
    for (index = 0; index < ONEK; index++){
        // set up all entry in directory to be not present first
        page_directory[index] = READ;
        // set up the page table, set the attribute READ and PRESENT
        page_table[index] = (index << 12) | READ ; 
        vid_page_table[index] = (index << 12) | READ ; 
    }
    // now put the first page table to the page directory
    page_directory[0] = ((uint32_t)page_table) | READ_P;
    // map the 4-8 MB in virtual memory to physical memory, this page is not cached.
    page_directory[1] = (FOURM | MSIZE | READ_P ); 
    // in lib.c, we found that the video memory is started at 0x83000
    page_table[(VIDEO>>12)] = VIDEO |READ | PRESENT;
    loadPageDirectory(page_directory);
    Enable_paging();

}

/*
 * map_process: map the virtual memory to the physical memory
 * 
 * Input: virtual_MEM: virtual memory 128 MB
 *        physical_MEM: physical memory 8 MB + process_num*4MB
 * Output: none
 * Return value: none
 * Side effect: map the page directory entry to another physical page
 * 
*/
void map_process(uint32_t virtual_MEM, uint32_t physical_MEM)
{   
    int32_t index;
    index = virtual_MEM/_4MB;
    page_directory[index] = physical_MEM | READ_P | MSIZE | USER;
    Flush_tlb(); 

}

/*
 * vid_mapping: map the virtual memory to the physical memory
 * 
 * Input: none
 * Output: none
 * Return value: none
 * Side effect: map the page directory entry to 160mb and set it right
*/
void vid_mapping(){
    page_directory[_160MB_idx] = ((uint32_t)vid_page_table) | READ_P| USER;
    vid_page_table[0] = VIDEO |READ_P | USER;
    Flush_tlb();
}

/*
 * terminal_mapping: map the terminal buffer to the corresponding 
 * place in the virtual mem.
 * Input: none
 * Output: the corresponding memory page is mapped
 * Return value: none
 * Side_effect: set up the certain entries in the first page table
 * 
*/

void terminal_mapping(){

    //page_directory[terminal0_buffer/_4KB] = ((uint32_t)terminal_vid_page_table) | READ_P| USER;
    page_table[(terminal0_buffer>>12)] = terminal0_buffer |READ_P ;
    page_table[(terminal1_buffer>>12)] = terminal1_buffer |READ_P ;   
    page_table[(terminal2_buffer>>12)] = terminal2_buffer |READ_P ;  //three terminals video memory mapping
    Flush_tlb();
}

/*
 * vid_remapping: remap the video to current process
 *
 * Input: the video buffer for current process
 * Output: the page directory entry
 * Return value: none
 * Side_effect: change the video mem in virtual memory mapping to the current process
 * 
*/
void vid_remapping(uint32_t physical_MEM){
    page_directory[_160MB_idx] = ((uint32_t)vid_page_table) | READ_P | USER;
    vid_page_table[0] = physical_MEM |READ_P | USER;
    Flush_tlb();
}

