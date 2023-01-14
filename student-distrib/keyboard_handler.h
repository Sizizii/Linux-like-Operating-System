#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H
#define KEYBOARD_PS2_PORT       0x60
#define keyboard_irq 0x01
#define LSHIFT_ON       0x2A
#define LSHIFT_OFF      0xAA
#define RSHIFT_ON       0x36
#define RSHIFT_OFF      0xB6
#define CAPS_ON          0x3A
#define CAPS_OFF         0xBA
#define CTRL_ON          0x1D
#define CTRL_OFF         0x9D
#define ALT_ON           0x38
#define ALT_OFF          0xB8

#define F1          		0x3B
#define F2          		0x3C
#define F3          		0x3D
#define ESC                 0x01
#define TAB                 0x0f
#define ENTER		        0x1C
#define BACKSPACE	        0x0E
#define key_size            60
#define buffer_size         127


// keyboard buffer
volatile unsigned char keyboard_buffer[buffer_size+1];
// keyboard buffer backup once the enter key is pressed
volatile unsigned char saved_keyboard_buffer[buffer_size+1];
// current buffer index
volatile uint8_t buffer_position, saved_buffer_position;

volatile int overflow_bits;
// initialize the keyboard
extern void init_keyboard();
// keyboard handler
extern void keyboard_handler();
// output the key value onto the screen
extern void output_keyvalue(unsigned char asc2_number);
// clear the keyboard buffer
extern void buffer_clear();
// get the enter flag
extern uint8_t return_enter_flag();
// reset the enter flag
extern void reset_enter_flag();
#endif
