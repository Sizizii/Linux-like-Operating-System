#include "lib.h"
#include "system_call.h"
#include "x86_desc.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "types.h"
#include "pcb.h"
#include "paging.h"
#include "scheduling.h"
// file_operation_t terminal   = {terminal_read, terminal_write, terminal_open, terminal_close};
file_operation_t rtc        = {rtc_read, rtc_write, rtc_open, rtc_close};
file_operation_t file       = {file_read, file_write, file_open, file_close};
file_operation_t dir        = {dir_read, dir_write, dir_open, dir_close};
// write function is invalid in stdin
file_operation_t stdin_op   = {terminal_read, error_write, terminal_open, terminal_close};
// read function is invalid in stdout
file_operation_t stdout_op  = {error_read, terminal_write, terminal_open, terminal_close};

// keep track of the current process and its parent process 
volatile uint32_t curr_pid = 0;
volatile uint32_t curr_parent_pid = 0;


/* int32_t error_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: -1
 * Function: fake read. always return -1 
*/
int32_t error_read(int32_t fd, void* buf, int32_t nbytes) {
    return -1;
}

/* int32_t error_write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: -1
 * Function: fake write. always return -1 
*/
int32_t error_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}


/* int32_t read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: number of data from buffer
 * Function:  
*/
// fd must between limit 0 to 8,buffer should not be a null pointer
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    if (fd < Minimum_open_files || fd > Maximum_open_files || buf == NULL || pcb_array[curr_pid]->fd_array[fd].flags == NOT_USE) { 
        return -1;
    }//all the conditions that read fails
    return pcb_array[curr_pid]->fd_array[fd].func_pointer->read(fd, buf, nbytes);
}

/* int32_t write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: number of data from buffer
 * Function: output content from buf 
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    if (fd < Minimum_open_files || fd >Maximum_open_files  || buf == NULL || pcb_array[curr_pid]->fd_array[fd].flags == NOT_USE) {
        return -1;
    }//all the conditions that write fails
    return pcb_array[curr_pid]->fd_array[fd].func_pointer->write(fd, buf, nbytes);
}


/* int32_t close(int32_t fd)
 * Inputs:  filename
 * Return Value: 0 on success -1 on failure
 * Function: end  
 * 
*/
int32_t close(int32_t fd) {
    file_descriptor_t* fd_pointer = &(pcb_array[curr_pid]->fd_array[fd]);
    if (fd < Minimum_open_files || fd > Maximum_open_files || fd_pointer->flags == NOT_USE) {
        return -1;
    }
    if (fd_pointer->func_pointer->close(fd) == 0) {
        fd_pointer->func_pointer = NULL;
        fd_pointer->inode_num = -1;
        fd_pointer->file_position = 0;
        fd_pointer->flags = NOT_USE;
        return 0;
    }
    return -1;
}


/* int32_t open(const uint8_t* filename)
 * Inputs:  filename
 * Return Value: fd on success -1 on failure
 * Function:  
*/
int32_t open(const uint8_t* filename) {
    int32_t fd;
    dentry_t new_dentry;
    pcb_t* pcb_pointer = pcb_array[curr_pid];

    /* NULL check */
    if(filename == NULL) {   
        printf("Filenme check failed!\n");
        return -1; 
    }

    // stdin should always be 0
    if (strncmp((int8_t*)filename, (int8_t*)"stdin", 5) == 0) {
        pcb_pointer->fd_array[STDIN_FD].flags = 1;
        pcb_pointer->fd_array[STDIN_FD].func_pointer = &stdin_op;
        return STDIN_FD;
    }

    // stdout should always be 1
    if (strncmp((int8_t*)filename, (int8_t*)"stdout", 6) == 0) {
        pcb_pointer->fd_array[STDOUT_FD].flags = 1;
        pcb_pointer->fd_array[STDOUT_FD].func_pointer = &stdout_op;
        return STDOUT_FD;
    }

    /* Exist file check */
    if (read_dentry_by_name(filename, &new_dentry) == -1) {
        printf("File doesn't exist!\n");
        return -1;
    }

    /* find an available file descriptor */
    for (fd = 2; fd < Maximum_open_files; fd++) {
        if(pcb_pointer->fd_array[fd].flags == 0) {    
            break;
        }
    }
    // if no available fd, return -1
    if (fd == Maximum_open_files)
        return -1;

    uint32_t filetype = new_dentry.filetype;
    // set up file descriptor by the file type
    if (filetype == RTC_filetype) {
        pcb_pointer->fd_array[fd].inode_num = -1;
        pcb_pointer->fd_array[fd].func_pointer = &rtc;
    } else if (filetype == directory_filetype) {
        pcb_pointer->fd_array[fd].inode_num = -1;
        pcb_pointer->fd_array[fd].func_pointer = &dir;
    } else if (filetype == regular_filetype) {
        pcb_pointer->fd_array[fd].inode_num = new_dentry.num_inode;
        pcb_pointer->fd_array[fd].func_pointer = &file;
    }

    // open the device
    if (pcb_pointer->fd_array[fd].func_pointer->open(filename) == -1) {
        // if failed, return -1
        pcb_pointer->fd_array[fd].inode_num = -1;
        pcb_pointer->fd_array[fd].func_pointer = NULL;
        return -1;
    }

    pcb_pointer->fd_array[fd].file_position = 0;
    pcb_pointer->fd_array[fd].flags = IN_USE;

	return fd;
}

/* getargs
 * Inputs:  buf -- a user-level buffer that stores the program's command line arguments
 *          nbytes -- number of bytes (args) to be copied
 * Outputs: None
 * Return Value: 0 for success, -1 for failure
 * Function: 
 *  copy arguments into user-level buffer
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
    /* NULL check */
    if(buf == NULL){ return -1; }
    
    /* get current pcb pointer */
    pcb_t* pcb_pointer = pcb_array[curr_pid];
    
    /* If no arguments, fail */
    if(pcb_pointer->args_buf[0] == '\0'){ return -1; }

    /* copy args */
    strncpy((int8_t*)buf, (int8_t*)(pcb_pointer->args_buf), nbytes);
    return 0;
}


/* parse_command
 * Inputs:  command
 * Outputs: file_name, request_command
 * Return Value: None
 * Function: 
 *  parse command and store the execution file name in file_name, 
 *  store the arg in request_command 
 */
void parse_command(const uint8_t* command, uint8_t* file_name, uint8_t* request_command) {
    int32_t start = 0;
    int32_t end;
    int i;
    /* decompose the command */
    // remove the leading spaces
    while (command[start] == ' ') {
        start++;
    }
    end = start;
    // stop when meet space or zero
    while (command[end] != ' ' && command[end] != '\0') {
        end++;
    }
    // copy the file name
    for (i = start; i < end && i-start < MAXIMUM_FILENAME_LENGTH-1; i++) {
        file_name[i-start] = command[i];
    }
    // add the end of string
    file_name[i-start] = '\0';
    // repeat the procedure above to get the args
    start = end;
    while (command[start] == ' ') {
        start++;
    }
    end = start;
    while (command[end] != ' ' && command[end] != '\0' ) {
        end++;
    }
    // copy the argument
    for (i = start; i < end && i-start < Maximum_command_length-1; i++) {
        request_command[i-start] = command[i];
    }
    // add the end of string
    request_command[i-start] = '\0';
    return;
}

/*
 * execute: execute the given command and check whether 
 * the file is executable
 * Input: command contains the file name to be executed and 
 * the paramters for the program, which are space-seperated
 * 
 * Output: none
 * Return value: -1 if the file is not executable, 0 if goes to halt, 256 if ended by exception
 * Side effect: execute the file
 * 
*/
int32_t execute(const uint8_t* command) {
    cli();
    // first separate the command into two parts
    uint8_t file_name[MAXIMUM_FILENAME_LENGTH];
    uint8_t request_command[Maximum_command_length];
    memset(file_name, 0, MAXIMUM_FILENAME_LENGTH);
    memset(request_command, 0, Maximum_command_length);
    int i;

    // check whether command is meaningful
    if (command == NULL) {
        sti();
        return -1;
    }

    // parse the command
    parse_command(command, file_name, request_command);

    /* read the file name and check whether it is executable*/
    dentry_t dentry;
    uint8_t buf[CHECK_VALID];
    
    if ( 0 != read_dentry_by_name(file_name,&dentry)){
        // no such file
        sti();
        return -1;
    }
    // check ELF
    read_data(dentry.num_inode,0,buf,CHECK_VALID);
    if (strncmp((int8_t*)buf, "\177ELF", CHECK_VALID) != 0) {
        sti();
        return -1;
    }

    // now find the first entry information;
    read_data(dentry.num_inode,first_entry,buf,CHECK_VALID);
    uint32_t top = *((uint32_t*)buf);

    // create new process
    uint32_t new_pid = create_process();
    if (new_pid == -1) {
        sti();
        return -1;
    }

    // get new pcb
    curr_parent_pid = curr_pid;
    curr_pid = new_pid;

    if(shell_tid > 2){
        // in normal case, the terminal switch current process
        terminals[pcb_array[curr_parent_pid]->terminal_number].cur_pid=curr_pid;       
        pcb_array[curr_pid]->terminal_number = pcb_array[curr_parent_pid]->terminal_number;
    }
    // when initialize, we set all shells' parent to be itself
    else{
        terminals[shell_tid].cur_pid=curr_pid;
        curr_parent_pid = curr_pid;
        pcb_array[curr_pid]->terminal_number = shell_tid;
    }
  

    pcb_t* current_PCB = bind_new_pcb(curr_pid, curr_parent_pid);


    // save esp and ebp to pcb
    asm volatile("			                \n\
			            movl %%ebp, %%eax 	\n\
			            movl %%esp, %%ebx 	\n\
			    "
			    :"=a"(current_PCB->saved_ebp), "=b"(current_PCB->saved_esp));

    // change the directory table
    map_process(_128MB, _8MB + curr_pid*FOURM);

    // map current process to buf or video
    if (pcb_array[curr_pid]->terminal_number == cur_display_tid){
        vid_remapping(VIDEO);
    }else{
        vid_remapping(terminals[pcb_array[curr_pid]->terminal_number].vid_buf);
    }

    // copy the data to 0x08048000 in virtual mem
    read_data(dentry.num_inode, 0, (uint8_t*)virtual_target, Maximum_file_length);

    // initialize file descriptor
    for (i = 0; i < Maximum_open_files; i++ ){
        current_PCB->fd_array[i].file_position = 0;
        current_PCB->fd_array[i].flags = NOT_USE;
        current_PCB->fd_array[i].func_pointer = NULL;
        current_PCB->fd_array[i].inode_num = -1;
    }

    //Initialize args
    strncpy((int8_t*)current_PCB->args_buf, (int8_t*)request_command, Maximum_command_length);

    // open stdin and stdout
    open((uint8_t*)"stdin");
    open((uint8_t*)"stdout");

    // Now is to do the context switching

    // Save SS0 and ESP0 in TSS 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - _8KB * (curr_pid) - 4;
    sti();
    /* set up the stack */
    // here the stack is the same as the hardware context structure
    asm volatile(
                 "cli;"
                 "movw $0x2B, %%ax;"
                 "movw %%ax, %%ds;"
                 "movl $0x83FFFFC, %%eax;"
                 "pushl $0x2B;"
                 "pushl %%eax;"
                 "pushfl;"
                 "popl %%edx;"
                 "orl $0x200, %%edx;"
                 "pushl %%edx;"
                 "pushl $0x23;"
                 "pushl %0;"
                 "iret;"
                 :	/* None for output */
                 :"r"(top)	/* to start at the top */
                 :"%eax","%edx"	/* clobber register */
                 );

    // due to the iret, the return value is in the eax
   
    return -1;
}

/*
 * restart_shell: restart the shell
 * Input: the shell id
 * Output: none
 * Return value: -1 if the file is not executable, 0 if goes to halt, 256 if ended by exception
 * Side effect: restart the shell
 * 
*/
int32_t restart_shell(int restart_shell_pid) {
    cli();
    int i;
    
    /* read the file name and check whether it is executable*/
    dentry_t dentry;
    uint8_t buf[CHECK_VALID];
    
    if ( 0 != read_dentry_by_name((uint8_t*)"shell",&dentry)){
        // no such file
        sti();
        return -1;
    }
    // check ELF
    read_data(dentry.num_inode,0,buf,CHECK_VALID);
    if (strncmp((int8_t*)buf, "\177ELF", CHECK_VALID) != 0) {
        sti();
        return -1;
    }

    // now find the first entry information;
    read_data(dentry.num_inode,first_entry,buf,CHECK_VALID);
    uint32_t top = *((uint32_t*)buf);

    // reuse the same pcb
    process_array[restart_shell_pid] = 1;
    curr_parent_pid = restart_shell_pid;
    curr_pid = restart_shell_pid;

    // when initialize, we set all shells' parent to be itself
    terminals[restart_shell_pid].cur_pid=restart_shell_pid;
    pcb_array[restart_shell_pid]->terminal_number = restart_shell_pid;
    pcb_t* current_PCB = bind_new_pcb(curr_pid, curr_parent_pid);


    // save esp and ebp to pcb
    asm volatile("			                \n\
			            movl %%ebp, %%eax 	\n\
			            movl %%esp, %%ebx 	\n\
			    "
			    :"=a"(current_PCB->saved_ebp), "=b"(current_PCB->saved_esp));

    // change the directory table
    map_process(_128MB, _8MB + curr_pid*FOURM);

    // map current process to buf or video
    if (pcb_array[curr_pid]->terminal_number == cur_display_tid){
        vid_remapping(VIDEO);
    }else{
        vid_remapping(terminals[pcb_array[curr_pid]->terminal_number].vid_buf);
    }

    // copy the data to 0x08048000 in virtual mem
    read_data(dentry.num_inode, 0, (uint8_t*)virtual_target, Maximum_file_length);

    // initialize file descriptor
    for (i = 0; i < Maximum_open_files; i++ ){
        current_PCB->fd_array[i].file_position = 0;
        current_PCB->fd_array[i].flags = NOT_USE;
        current_PCB->fd_array[i].func_pointer = NULL;
        current_PCB->fd_array[i].inode_num = -1;
    }

    //Initialize args
    strncpy((int8_t*)current_PCB->args_buf, (int8_t*)"", Maximum_command_length);

    // open stdin and stdout
    open((uint8_t*)"stdin");
    open((uint8_t*)"stdout");

    // Now is to do the context switching

    // Save SS0 and ESP0 in TSS 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - _8KB * (curr_pid) - 4;
    sti();
    /* set up the stack */
    // here the stack is the same as the hardware context structure
    asm volatile(
                 "cli;"
                 "movw $0x2B, %%ax;"
                 "movw %%ax, %%ds;"
                 "movl $0x83FFFFC, %%eax;"
                 "pushl $0x2B;"
                 "pushl %%eax;"
                 "pushfl;"
                 "popl %%edx;"
                 "orl $0x200, %%edx;"
                 "pushl %%edx;"
                 "pushl $0x23;"
                 "pushl %0;"
                 "iret;"
                 :	/* None for output */
                 :"r"(top)	/* to start at the top */
                 :"%eax","%edx"	/* clobber register */
                 );

    // due to the iret, the return value is in the eax
   
    return -1;
}

/*
 * halt: halt the program and return to the parent process
 * Input: program exit code
 * Output: none
 * Return value: exit code
 * 
*/
int32_t halt(uint8_t exit_code) {
    cli();
    uint32_t saved_esp = pcb_array[curr_pid]->saved_esp;
    uint32_t saved_ebp = pcb_array[curr_pid]->saved_ebp;

    // free the current pcb
    close_pcb(curr_pid);
    // if it has no parent pid, which means it is the shell, restart the shell
    if (curr_pid == curr_parent_pid) {
        // sti();
        // execute((uint8_t*)"shell");
        restart_shell(curr_pid);
    }

    // restore the parent pcb
    curr_pid = curr_parent_pid;
    curr_parent_pid = pcb_array[curr_pid]->parent_pid;
    terminals[pcb_array[curr_pid]->terminal_number].cur_pid=curr_pid;//csx add terminal
    // remap the memory to the parent process
    // * flush tlb already in it
    map_process(_128MB, _8MB + curr_pid * _4MB);

    // restore ss0 and esp0
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - curr_pid * _8KB - 4;

    // restore the stack and put the return value onto eax
    asm volatile(
        "mov %0,%%eax;"
        "mov %1,%%esp;"
        "mov %2,%%ebp;"
        :
        :
        "r"((uint32_t)exit_code), "r"(saved_esp), "r"(saved_ebp)
        :"%eax"
    );
    sti();
    asm volatile("leave;ret;");
    return 0;
}


/*
 * vidmap: maps the text-mode video memory into user space 160MB
 * Input: uint8_t** screen_start
 * Return value: -1 if pointer address is wrong,0 if success
 * 
*/
int32_t vidmap(uint8_t** screen_start){
    if(screen_start==NULL){
        return -1;
    }

    //To avoid adding kernel-side exception handling


    //check if the pointer is not in user code space
    if(screen_start<(uint8_t**)_128MB || screen_start>=(uint8_t**)_132MB){
        return -1;
    }
    *screen_start=(uint8_t*)_160MB;//put the start place in 
    vid_mapping();//create the 4k page at address 160MB   

 
    return 0;
}

/*
 * terminalmap: maps the text-mode video memory into user space 160MB
 * Input: uint8_t** screen_start
 * Return value: -1 if pointer address is wrong,0 if success
 * 
*/
int32_t terminalmap(uint8_t** screen_start,uint32_t terminal_address,int tid){
    if(screen_start==NULL){
        return -1;
    }

    //To avoid adding kernel-side exception handling


    //check if the pointer is not in user code space
    if(screen_start<(uint8_t**)_128MB || screen_start>=(uint8_t**)_132MB){
        return -1;
    }
    *screen_start=(uint8_t*)_160MB;//put the start place in 
    vid_remapping(terminals[tid].vid_buf);//create the 4k page at address 160MB   

 
    return 0;
}
/*
 * set_handler: set custom handler
 * Input: signum and handler_address
 * Return value: always -1 as it is not supported yet
 * 
*/
int32_t set_handler(int32_t signum, void* handler_address) {
    return -1;
}

/*
 * sigreturn: return from signal
 * Input: none
 * Return value: always -1 as it is not supported yet
 * 
*/
int32_t sigreturn(void) {
    return -1;
}

