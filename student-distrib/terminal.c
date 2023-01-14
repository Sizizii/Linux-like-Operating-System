#include "terminal.h"



terminal_t terminals[Terminal_total_num];
volatile int32_t cur_display_tid;

/* void init_terminal
 * Inputs:  None
 * Return Value: None
 * Function: Initialize terminal arrays*/

void init_terminal(){
    int i,j;
    terminals[0].vid_buf = terminal0_buffer;
    terminals[1].vid_buf = terminal1_buffer;
    terminals[2].vid_buf = terminal2_buffer;    
    terminal_mapping(); //initialize
    for(i=0; i < Terminal_total_num; i++){
        terminals[i].tid = i;
        terminals[i].cur_pid = -1;
        for(j=0;j<=buffer_size;j++){
            terminals[i].terminal_buffer[j]=0; //clear each position of buffer
        }
        terminals[i].buffer_position=0; //set the start position of write to zero 
        // terminals[i].saved_buffer_position=0;
        terminals[i].enter_flag=0;
        for (j = 0; j < SCREEN_SIZE; j++)
        {
            *(uint8_t *)((uint8_t *)terminals[i].vid_buf +(j << 1) ) = ' ';//clear physical memory for terminal storage
            *(uint8_t *)((uint8_t *)terminals[i].vid_buf +(j << 1) + 1) = ATTRIB;
        }        
        terminals[i].x_position=0; //reset all cursors
        terminals[i].y_position=0;    
    }


    buffer_clear();
    buffer_position = 0;

    clear_screen();
    cur_display_tid = 0;//first terminal display

};

/* void terminal_switch
 * Inputs:  int32_t terminal_number
 * Return Value: 0 for success, -1 for failure
 * Function: change display terminal*/

int32_t terminal_switch(int32_t terminal_number){
    int i;
    cli();
    //return failure if the input terminal number is wrong
    if(terminal_number < 0 || terminal_number > Terminal_max_num){
        sti();
        return -1;
    }
    if(terminal_number == cur_display_tid){
        sti();
        return 0;//not changing
    }

    // save the current terminal information
    terminals[cur_display_tid].x_position = get_x();
    terminals[cur_display_tid].y_position = get_y();
    terminals[cur_display_tid].buffer_position = buffer_position;
    for(i=0;i<=buffer_size;i++){
        terminals[cur_display_tid].terminal_buffer[i] = keyboard_buffer[i]; //clear each position of buffer
    }
    memcpy((uint8_t *)terminals[cur_display_tid].vid_buf,(uint8_t *)VIDEO,_4KB);

    cur_display_tid = terminal_number;
    //if the switched terminal is not set yet
    buffer_clear();//clear the keyboard buffer first   
    for(i=0;i<=buffer_size;i++){
        keyboard_buffer[i] = terminals[cur_display_tid].terminal_buffer[i]; //clear each position of buffer
    }         
    buffer_position = terminals[cur_display_tid].buffer_position;

    memcpy((uint8_t *)VIDEO,(uint8_t *)terminals[cur_display_tid].vid_buf,_4KB);
    //check if the current running terminal is the one wo switch to
    if(curr_pid == terminals[cur_display_tid].cur_pid){
        // if current pid is in the current display, we show the change to screen
        vid_remapping(VIDEO);
    }    
    else{
        // if current pid no more in the display terminal, we change it to modify the buffer
        vid_remapping(terminals[pcb_array[curr_pid]->terminal_number].vid_buf);
    }
    change_x(terminals[cur_display_tid].x_position);
    change_y(terminals[cur_display_tid].y_position);    
    move_cursor(terminals[cur_display_tid].x_position,terminals[cur_display_tid].y_position);//refresh cursor position
    sti();
    return 0;
}


/* int32_t terminal_close(int32_t fd)
 * Inputs:  filename
 * Return Value: 0 on success -1 on failure
 * Function: end terminal */
int32_t terminal_close(int32_t fd) {
    return -1;
}


/* int32_t terminal_open(const uint8_t* filename)
 * Inputs:  filename
 * Return Value: 0 on success -1 on failure
 * Function: build terminal */
int32_t terminal_open(const uint8_t* filename) {
	return 0;
}


/* int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: number of data from buffer
 * Function: put content from saved_keyboard_buffer to buf */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    if (buf == NULL)
        return -1;
    uint8_t* buffer = (uint8_t*) buf;
    int char_number=0;
    int i;
    while(1){
        if(1==terminals[pcb_array[curr_pid]->terminal_number].enter_flag){
            saved_keyboard_buffer[saved_buffer_position]='\n'; // \n will occur in the end of keyboard buffer
            saved_buffer_position++;
            break;
        }
    }
    for(i=0;i<=buffer_size;i++){
        buffer[i]=saved_keyboard_buffer[i]; //put content from saved_keyboard_buffer to buf
        if('\n'==saved_keyboard_buffer[i] || i == nbytes-1){//detect whether saved_keyboard_buffer ends or nbytes is satisfied
            char_number = i+1;
            break;
        }
    }
    terminals[pcb_array[curr_pid]->terminal_number].enter_flag=0;//after all ends clear flag
    //clear 
    buffer_clear();
    for(i=0;i<=buffer_size;i++){
        terminals[pcb_array[curr_pid]->terminal_number].terminal_buffer[i]=0; //clear each position of buffer
    }
    terminals[pcb_array[curr_pid]->terminal_number].buffer_position=0; //set the start position of write to zero 
    return char_number;
}


/* int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: number of data from buffer
 * Function: output content from buf */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t cur_terminal;
    if (buf == NULL)
        return -1;
    uint8_t* buffer = (uint8_t*)buf;    
    int32_t i;

    cli();//block interrupt

    cur_terminal = pcb_array[curr_pid]->terminal_number;

    // if current execute terminal is on display, simply write on the video memory
    if(cur_display_tid == cur_terminal){
        for(i=0;i<nbytes;i++){
            if(i==0){
                special_putc('\0',buffer[i]);//check if it is the first element in buffer
            }
            else{
                special_putc(buffer[i-1],buffer[i]);
            }
        }
    }
    // if current execute terminal is not on display, just write to the
    else{
        for(i=0;i<nbytes;i++){
            if(i==0){
                terminal_special_putc('\0',buffer[i],cur_terminal);//check if it is the first element in buffer
            }
            else{
                terminal_special_putc(buffer[i-1],buffer[i],cur_terminal);
            }            
        }
    }
        

    sti();
    return i;
}
