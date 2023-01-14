#ifndef TERMINAL_H
#define TERMINAL_H
#include "lib.h"
#include "keyboard_handler.h"
#include "paging.h"
#include "system_call.h"
#define Terminal_total_num  3
#define Terminal_max_num 2
#define SCREEN_SIZE 2000
#define ATTRIB      0x7
/* Multiple terminals */
typedef struct terminal_t{
    int32_t    tid; 
    int32_t    cur_pid;
    uint32_t   vid_buf;
    int32_t    x_position;
    int32_t    y_position;
    volatile unsigned char terminal_buffer[buffer_size+1];
    volatile uint8_t buffer_position;
    int32_t    enter_flag; 
}terminal_t;

extern terminal_t terminals[Terminal_total_num];
extern volatile int32_t cur_display_tid;

void init_terminal();

/* Terminal system calls */
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_switch(int32_t terminal_number);
#endif
