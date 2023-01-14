#ifndef _SYSTEM_CALL_H
#define _SYSTEM_CALL_H

#include "x86_desc.h"
#include "paging.h"
#include "types.h"
#include "lib.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"

#define Maximum_open_files 8
#define Minimum_open_files 0
// when execute die by exception 
#define Exception 256
// number of process
#define Process 6
#define Maximum_command_length 128
#define CHECK_VALID 4
#define MAGIC_1 0x7f
#define MAGIC_2 0x45
#define MAGIC_3 0x4c
#define MAGIC_4 0x46
#define _128MB  0x08000000
#define _132MB  0x08400000
#define _160MB  0x0a000000
#define _164MB  0x0a400000
#define _4MB    0x00400000
#define _8MB    0x00800000
#define _8KB    0x00002000
#define _4KB    0x00001000
#define first_entry 24

#define RTC_filetype 0
#define directory_filetype 1
#define regular_filetype 2
#define STDIN_FD 0
#define STDOUT_FD 1

#define virtual_target 0x08048000
#define Maximum_file_length 0x08000000
#define Maximum_args_size 128
#define IN_USE 1
#define NOT_USE 0

// file operation table
typedef struct file_operation_t {
	int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
	int32_t (*open)(const uint8_t* filename);
	int32_t (*close)(int32_t fd);
} file_operation_t;


// file descriptor
typedef struct file_descriptor_t {
    file_operation_t*   func_pointer;
    uint32_t            inode_num;
    int32_t             file_position;
    int32_t             flags;
} file_descriptor_t;

typedef struct pcb_t {
    uint32_t            pid;
    uint32_t            parent_pid;
    file_descriptor_t   fd_array[Maximum_open_files];
    uint32_t            saved_esp;
    uint32_t            saved_ebp;
    uint8_t             args_buf[Maximum_args_size];
    uint8_t             terminal_number;
    uint32_t            own_esp;
    uint32_t            own_ebp;
    int32_t             rtcfreq;
    int32_t             rtc_counter;
} pcb_t;

int32_t process_array[Process];
pcb_t* pcb_array[Process];

extern volatile uint32_t curr_pid;
extern volatile uint32_t curr_parent_pid;


/* execute the file */
int32_t execute(const uint8_t* command);
// halt the process
int32_t halt(uint8_t exit_code);

// systemcall functions
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t close(int32_t fd);
int32_t open(const uint8_t* filename);
int32_t vidmap(uint8_t** screen_start);
int32_t terminalmap(uint8_t** screen_start,uint32_t terminal_address,int tid);
/* create a process */
int32_t create_process();

// some fake read/write functions. always return -1
int32_t error_read(int32_t fd, void* buf, int32_t nbytes);
int32_t error_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
