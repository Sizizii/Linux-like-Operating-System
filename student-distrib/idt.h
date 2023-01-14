#ifndef IDT_H
#define IDT_H

#define EXCEPTION_NUM 20
#define IDT_NUM 256
#define SYSTEMCALL_ID 0x80
#define KEYBOARD_VEC_NUM 0x21
#define RTC_VEC_NUM 0x28
#define PIT_VEC_NUM 0x20

// set up the IDT table
extern void idt_setup();

// exception handlers
void exception_handler_00();
void exception_handler_01();
void exception_handler_02();
void exception_handler_03();
void exception_handler_04();
void exception_handler_05();
void exception_handler_06();
void exception_handler_07();
void exception_handler_08();
void exception_handler_09();
void exception_handler_10();
void exception_handler_11();
void exception_handler_12();
void exception_handler_13();
void exception_handler_14();
void exception_handler_15();
void exception_handler_16();
void exception_handler_17();
void exception_handler_18();
void exception_handler_19();
void idt_default_handler();

// keyboard handler is in keyboard_handler.h
// RTC handler is in rtc.h


#endif /* IDT_H */

