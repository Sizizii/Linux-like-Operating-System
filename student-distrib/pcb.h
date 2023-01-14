#ifndef _PCB_H
#define _PCB_H

#include "types.h"
#include "system_call.h"

#define MAX_PROCESS_NUM     6
#define MAX_FD_NUM          8

#define EIGHT_KB            8192
#define EIGHT_MB            (EIGHT_KB * 1024)

// initialize the pcb
void init_pcb();
// bind the pid to a pcb
pcb_t* bind_new_pcb(uint32_t pid, uint32_t parent_pid);
// destroy the pcb
void close_pcb(uint32_t pid);
// get a new pid
int32_t create_process();

#endif
