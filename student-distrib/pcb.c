#include "pcb.h"

/*
 * create_process: check whether the process_array is full, if not,
 * return the available process number
 * Input: none
 * Output: none
 * Return Value: the available process number or -1 for failure
 * Side_effect: change the process array
 * 
 * 
*/
int32_t create_process() {
    int i;
    for(i = 0; i < Process; i++) {
        if (process_array[i] == 0) {
            process_array[i] = 1;
            return i;
        }
    }
    return -1;
}


/*
 * init_pcb: initialize the pcb arrays
 * Input: none
 * Output: none
 * Return Value: none
 * Side_effect: pcb array filled with initialized values
 * 
 * 
*/
void init_pcb() {
    int i, j;
    pcb_t* curr_pcb = NULL;
    // six process in total
    for (i = 0; i < MAX_PROCESS_NUM; i++) {
        // clear the corresponding pid index
        process_array[i] = 0;
        // allocate memory position
        pcb_array[i] = (pcb_t*)(EIGHT_MB - EIGHT_KB * (1 + i));
        curr_pcb = pcb_array[i];
        // set init values for esp and ebp
        curr_pcb->saved_esp = (EIGHT_MB - EIGHT_KB * i + 1);
        curr_pcb->saved_ebp = curr_pcb->saved_esp;
        // set init values for rtc variables
        curr_pcb->rtcfreq = 0;
        curr_pcb->rtc_counter = 0;
        // 0 and 1 are stdin and stdout, thus no initialization is needed.
        for (j = 2; j < MAX_FD_NUM; j++) {
            curr_pcb->fd_array[j].func_pointer = NULL;
            curr_pcb->fd_array[j].inode_num = -1;
            curr_pcb->fd_array[j].file_position = 0;
            curr_pcb->fd_array[j].flags = 0;
        }
    }
    return;
}

/*
 * bind_new_pcb: bind a new pcb to a given pid
 * Input: pid and its parent pid
 * Output: none
 * Return Value: the binded pcb pointer
 * Side_effect: allocate a pcb to the given pid
 * 
 * 
*/
pcb_t* bind_new_pcb(uint32_t pid, uint32_t parent_pid) {
    pcb_array[pid]->pid = pid;
    // first shell do not have parent pid
    if (pid == 0) {
        pcb_array[pid]->parent_pid = pid;
    } else {
        pcb_array[pid]->parent_pid = parent_pid;
    }
    return (pcb_t*) (EIGHT_MB - EIGHT_KB * (1 + pid));
}


/*
 * close_pcb: free the pcb
 * Input: pid and its parent pid
 * Output: none
 * Return Value: none
 * Side_effect: free the pcb given a pid
 * 
 * 
*/
void close_pcb(uint32_t pid) {
    process_array[pid] = 0;
    pcb_array[pid]->pid = 0;
    pcb_array[pid]->parent_pid = 0;
    pcb_array[pid]->terminal_number = -1;

    /* reinit rtc */
    pcb_array[pid]->rtcfreq = 0;
    pcb_array[pid]->rtc_counter = 0;

    int i;
    for (i = 0; i < MAX_FD_NUM; i++) {
        if (pcb_array[pid]->fd_array[i].flags == 1) {
            pcb_array[pid]->fd_array[i].func_pointer->close(i);
            pcb_array[pid]->fd_array[i].func_pointer = NULL;
        }
        pcb_array[pid]->fd_array[i].flags = 0;
    }
    return;
}
