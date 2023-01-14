
#include "lib.h"
#include "i8259.h"
#include "keyboard_handler.h"
#include "types.h"
#include "terminal.h"
#define KEYBOARD_PS2_PORT       0x60

#define CAPS_ON          0x3A
#define CAPS_OFF         0xBA

#define mode 4


volatile uint8_t caps_flag = 0;//0 means release, 1 means press
volatile uint8_t enter_flag = 0;
volatile uint8_t shift_flag = 0;
volatile uint8_t ctrl_flag  = 0;
volatile uint8_t alt_flag  = 0;

/* uint8_t return_enter_flag()
 * Inputs:  void
 * Return Value: uint8_t enter_flag
 * Function: get enter_flag value */
uint8_t return_enter_flag(){
    return enter_flag;
}
/* void reset_enter_flag()()
 * Inputs:  void
 * Return Value: void
 * Function: reset enter_flag value */
void reset_enter_flag(){
    enter_flag=0;
}
// volatile uint8_t *keyboard_buffer;
// volatile uint8_t buffer_position;

//this is the keyboard reference for each key
static uint8_t keyboard_reference[mode][key_size] = {
    //origin    
    {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 
     '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
     '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,0},
    //press caps
    {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
     '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
     '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
     0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0,0},
    //press shift
    {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 
    '\n', 0, 'A', 'S','D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', 
     0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',  0, ' ', 0,0},
    //press shift when caps+flag is on
    {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 
    '\b', '\t','q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', 
    '\n', 0, 'a', 's','d', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', 
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', 0, '*', 0, ' ', 0,0}
};


/* init_keyboard: initialize the keyboard
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the keyboard interrupter
*/

void init_keyboard(){
    enable_irq(keyboard_irq);//UNMASK THE KEYBOARD ON pic
}


/* keyboard_handler: the interrupt handler of keyboard
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: echo if you press a key
*/
void keyboard_handler(){
    cli();
    unsigned char buffer;
    buffer=inb(KEYBOARD_PS2_PORT);
    // if(buffer_position==buffer_size){
    //     return;
    // }
    if(buffer==LSHIFT_OFF ||buffer==RSHIFT_OFF){
        shift_flag=0;
    }    
    if(buffer==CTRL_OFF){
        ctrl_flag=0;
    }
    
    if(buffer==ALT_OFF){
        alt_flag=0;
    }
    if(1==alt_flag && buffer==F1){
        send_eoi(keyboard_irq);
        terminal_switch(0);
        
        return;
    }       
    else if(1==alt_flag && buffer==F2){
        send_eoi(keyboard_irq);
        terminal_switch(1);
        
        return;
    }      
    else if(1==alt_flag && buffer==F3){
        send_eoi(keyboard_irq);
        terminal_switch(2);
        
        return;
    }       
    // to check which buffer should we write to
    if(buffer < key_size){
        if(buffer==CAPS_ON){
                if(caps_flag==0){ //flag to switch keyboard mode
                    caps_flag=1;
                } 
                else {
                    caps_flag=0;
                }            
        }
        else if(buffer==LSHIFT_ON ||buffer==RSHIFT_ON){
            shift_flag=1;
        }
        // else if(buffer==LSHIFT_OFF ||buffer==RSHIFT_OFF){
        //     putc('@');
        //     shift_flag=0;
        // }
        else if(buffer==CTRL_ON){
            ctrl_flag=1;
        }
        // else if(buffer==CTRL_OFF){
        //     ctrl_flag=0;
        // }
        else if(buffer==ALT_ON){
            alt_flag=1;
        }        
        else if(keyboard_reference[0][buffer]=='\n'){
            overflow_bits=0;
            terminals[cur_display_tid].enter_flag = 1;
            memcpy((void*)saved_keyboard_buffer, (void*)keyboard_buffer, buffer_size+1);
            saved_buffer_position = buffer_position;
            buffer_clear();
        }
        output_keyvalue(buffer); //go to print the buffer on screen         
    }
    sti();
    send_eoi(keyboard_irq);
}


/* output_keyvalue: print different type of key press
 * 
 * Input: unsigned char asc2_number
 * Output: none
 * return: none
 * side_effect: echo if you press a key
*/
void output_keyvalue(unsigned char asc2_number){
    unsigned char letter;
    //check if the asc2_number is backspace
    if(keyboard_reference[0][asc2_number]=='\b'){
        //check whether this is not the first in buffer

        if(buffer_position!=0){
            unsigned char temp=keyboard_buffer[buffer_position-1];
            keyboard_buffer[buffer_position]=0;//clear previous buffer char
            buffer_position=buffer_position-1;
            special_putc(temp,'\b');
            return;
        }
        if(overflow_bits!=0){
            overflow_bits--;
            special_putc('\0','\b');
        }
        // special_putc('\0','\b');
        return;            
    }
    if(asc2_number==CAPS_ON||asc2_number==CAPS_OFF){
        return;
    }
    if(asc2_number==LSHIFT_OFF||asc2_number==RSHIFT_OFF){
        shift_flag=0;
        return;
    }
    if(asc2_number==ALT_OFF||asc2_number==ALT_ON){
        return;
    }

    //ctrl+l to clear the screen
    if(keyboard_reference[0][asc2_number]=='l' && ctrl_flag==1){
        clear();
        init_screen();
        move_cursor(0,0);
        return;
    }
 
    if (0==caps_flag && 0==shift_flag){ //print  numbers and lower case letters
        letter = keyboard_reference[0][asc2_number];
    }
    else if(1==caps_flag && 0==shift_flag){//print  numbers and higher case letters
        letter = keyboard_reference[1][asc2_number];
    }
    else if(0==caps_flag && 1==shift_flag){//print symbols and higher case letters
        letter = keyboard_reference[2][asc2_number];
    }
    else if(1==caps_flag && 1==shift_flag){//print symbols and lower case letters
        letter = keyboard_reference[3][asc2_number];
    }

    if(letter!=0){
        if(buffer_position<buffer_size){// if full, do nothing, prevent overflow
            keyboard_buffer[buffer_position]=letter;
            buffer_position++;
        }
        else{//overflow happens
            overflow_bits++;
        }
        special_putc('\0',letter);// output letter
    }
}
/* void buffer_clear(void)
 * Inputs:  void
 * Return Value: void
 * Function: clear keyboard_buffer, reset position */
void buffer_clear(){
    int i;
    for(i=0;i<=buffer_size;i++){
        keyboard_buffer[i]=0; //clear each position of buffer
    }
    buffer_position=0; //set the start position of write to zero 
}
