#include "scheduling.h"
#include "i8259.h"
#include "paging.h"

volatile int32_t active_tid = 0;
volatile int32_t next_sched_tid;
volatile int32_t shell_tid =0;
/*
 * init_PIT: initialize PIT, set up the I/O port
 * and enable the interrupt
 * Input: none
 * Output: none
 * Return value: none
 * Side_effect: enable the IRQ_0
 * 
*/
void init_PIT(){
    /* Tell PIT that we're setting CHANNEL0 */
    outb(PIT_MODE_REG, PIT_COMMAND_PORT);
    /* Send low byte */
    outb(PIT_FREQ_DIVISOR & HIGH_BYTE_MASK, PIT_CHANNEL0);
    /* Send high byte */
    outb(PIT_FREQ_DIVISOR >> HIGH_BITS_SHIFT, PIT_CHANNEL0);
    enable_irq(PIT_IRQ);
}

/*
 * pit_handler: when the interrupt comes, switch the process
 * Input: none
 * Output: switch the process
 * Return value: none
 * Side_effect: check the PIT interrupt and call process switch
 * 
*/
void pit_handler(){
    send_eoi(PIT_IRQ);
    cli();
    // initialize the first three shell
    if(shell_tid<=2){
        active_tid=shell_tid;
        if(terminals[shell_tid].cur_pid == -1 && shell_tid == 0){
            sti();
            execute((uint8_t *)"shell");
            return;
        }
        else if (terminals[shell_tid].cur_pid == -1)
        {
            process_switch(shell_tid);
            sti();
            return;
        }
        else{
            shell_tid++;
            sti();
            return;
        }        
    }
    // since the process is volatile, we don't want any interrupt when switch process

    int32_t next_process = reschedule();
    process_switch(next_process);
    sti();
}

/*
 * process_switch: switch from current process to the nect process,
 * store the previous information to PCB and get the new PCB based on 
 * the process id
 * Input: none
 * Output: switch the process, change the PCB
 * return value: 0 means success, -1 means fail
 * Side_effect: switch the process
 * 
*/
void process_switch(int32_t next_pid){
    // use for the shell initialize
    if (shell_tid <= 2){
        pcb_t* shell_pcb = pcb_array[next_pid-1];
    /* First store contents to enable restoration of the cur process state for the next time slice */
        asm volatile(			                
			    "movl %%ebp, %%eax;" 	
			    "movl %%esp, %%ebx;" 	
			    :"=a"(shell_pcb->own_ebp), "=b"(shell_pcb->own_esp)
                :
                );   /* outputs */
        execute((uint8_t *)"shell");
        return;
    }

    terminal_t next_terminal = terminals[next_sched_tid];

    pcb_t* cur_pcb = pcb_array[curr_pid];
    pcb_t* next_pcb = pcb_array[next_pid];
    

    /* Video memory mapping */
    /* If the next scheduled terminal is currently displayed */
    if(cur_display_tid == next_sched_tid){
        // user can draw on the video directly
        vid_mapping();
    }else{
        // otherwise user will draw on the buffer
        vid_remapping((uint32_t)next_terminal.vid_buf);
    }

    /* Set up paging for the next process */
    map_process(PROGRAM_IMAGE, _8MB + _4MB * next_pid);

    curr_pid = next_pid;
    curr_parent_pid = pcb_array[next_pid]->parent_pid;
    /* Save SS0 and ESP0 in TSS */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - _8KB * (curr_pid) - 4;
    active_tid = next_sched_tid;
    /* context switch */
    /* First store contents to enable restoration of the cur process state for the next time slice */
    asm volatile(			                
			    "movl %%ebp, %%eax;" 	
			    "movl %%esp, %%ebx;" 	
			    :"=a"(cur_pcb->own_ebp), "=b"(cur_pcb->own_esp)
                :
                );   /* outputs */

    /* Update current running terminal and stack info */
    asm volatile(			                
			    "movl %%eax, %%ebp;" 	
			    "movl %%ebx, %%esp;" 	
                :
			    :"a"(next_pcb->own_ebp), "b"(next_pcb->own_esp)
                );   /* inputs */
    return;
}

/*
 * reschedule: decide which process should be executed next and  
 * rearrange the schedule list
 * Input: none
 * Output: none
 * Return value: process id of next process
 * Side_effect: rearrange the next process
 * 
*/
int32_t reschedule(){
    // find the next terminal that should be executed
    next_sched_tid = (active_tid + 1) % terminal_total_num;
    // in the next terminal, figure out which process should be executed
    terminal_t next_terminal = terminals[next_sched_tid];
    int32_t next_sched_pid = next_terminal.cur_pid;
    return next_sched_pid;
}















