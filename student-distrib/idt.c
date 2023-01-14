#include "idt.h"
#include "lib.h"
#include "x86_desc.h"
#include "types.h"
#include "handler_link.h"
#include "rtc.h"

// exception description table
static char* EXCEPTION_TABLE[EXCEPTION_NUM] = {
    " 0:Divide-by-zero",
    " 1:Debug ",
    " 2:Non-maskable Interrupt",
    " 3:Breakpoint",
    " 4:Overflow",
    " 5:Bound Range Exceeded",
    " 6:Invalid Opcode",
    " 7:Device Not Available",
    " 8:Double Fault",
    " 9:Coprocessor Segment Overrun",
    "10:Invalid TSS",
    "11:Segment Not Present",
    "12:Stack Segment Fault",
    "13:General Protection Fault",
    "14:Page Fault",
    "15: Spurious Interrupt",
    "16: x87 Floating-Point Exception",
    "17: Alignment Check",
    "18: Machine Check",
    "19: SIMD Floating-Point Exception"
};


/* exception_handler_00: initialize the exception handler 0
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_00(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",0,EXCEPTION_TABLE[0]);
    
    while(1);sti();
    }
/* exception_handler_01: initialize the exception handler 1
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_01(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",1,EXCEPTION_TABLE[1]);
    
    while(1);sti();
    }
/* exception_handler_02: initialize the exception handler 2
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_02(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",2,EXCEPTION_TABLE[2]);
    
    while(1);sti();
    }
/* exception_handler_03: initialize the exception handler 3
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_03(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",3,EXCEPTION_TABLE[3]);
    
    while(1);sti();
    }
    /* exception_handler_04: initialize the exception handler4
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_04(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",4,EXCEPTION_TABLE[4]);
    
    while(1);sti();
    }
    /* exception_handler_05: initialize the exception handler 5
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_05(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",5,EXCEPTION_TABLE[5]);
    
    while(1);sti();
    }
    /* exception_handler_06: initialize the exception handler6
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_06(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",6,EXCEPTION_TABLE[6]);
    
    while(1);sti();
    }
    /* exception_handler_07: initialize the exception handler 7
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_07(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",7,EXCEPTION_TABLE[7]);
    
    while(1);sti();
    }
/* exception_handler_08: initialize the exception handler 8
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_08(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",8,EXCEPTION_TABLE[8]);
    while(1);
    sti();
    }
/* exception_handler_09: initialize the exception handler 9
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_09(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",9,EXCEPTION_TABLE[9]);
    while(1);
    sti();
    }
/* exception_handler_10: initialize the exception handler 10
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_10(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",10,EXCEPTION_TABLE[10]);
    while(1);
    sti();
    }
/* exception_handler_11: initialize the exception handler 11
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_11(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",11,EXCEPTION_TABLE[11]);
    while(1);
    sti();
    }
/* exception_handler_12: initialize the exception handler 12
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/   
void exception_handler_12(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",12,EXCEPTION_TABLE[12]);
    while(1);
    sti();
    }
/* exception_handler_13: initialize the exception handler 13
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_13(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",13,EXCEPTION_TABLE[13]);
    while(1);
    sti();
    }
/* exception_handler_14: initialize the exception handler 14
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_14(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",14,EXCEPTION_TABLE[14]);
    while(1);
    sti();
    }
/* exception_handler_15: initialize the exception handler 15
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_15(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",15,EXCEPTION_TABLE[15]);
    
    while(1);sti();
    }
/* exception_handler_16: initialize the exception handler 16
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_16(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",16,EXCEPTION_TABLE[16]);
    
    while(1);sti();
    }
/* exception_handler_17: initialize the exception handler 17
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_17(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",17,EXCEPTION_TABLE[17]);
    
    while(1);sti();
    }
/* exception_handler_18: initialize the exception handler 18
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_18(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",18,EXCEPTION_TABLE[18]);
    
    while(1);sti();
    }
/* exception_handler_19: initialize the exception handler 19
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the exception handler
*/
void exception_handler_19(){
    cli(); //for exception, directly mask the eflags
    printf("exception %x : %s happens",19,EXCEPTION_TABLE[19]);
    
    while(1);sti();
    }
/* idt_default_handler: default exception handler
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the default exception handler
*/
void idt_default_handler(){
    cli(); //for exception, directly mask the eflags
    printf("default handler.\n");
    
    while(1);sti();
    }


/* idt_setup: initialize the idt
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: set up the idt with handlers
*/
void idt_setup(){
    //idt is the global pointer of the table address
    // set up the handler entries
    SET_IDT_ENTRY(idt[0],  &exception_handler_linkage_00);
    SET_IDT_ENTRY(idt[1],  &exception_handler_linkage_01);
    SET_IDT_ENTRY(idt[2],  &exception_handler_linkage_02);
    SET_IDT_ENTRY(idt[3],  &exception_handler_linkage_03);
    SET_IDT_ENTRY(idt[4],  &exception_handler_linkage_04);
    SET_IDT_ENTRY(idt[5],  &exception_handler_linkage_05);
    SET_IDT_ENTRY(idt[6],  &exception_handler_linkage_06);
    SET_IDT_ENTRY(idt[7],  &exception_handler_linkage_07);
    SET_IDT_ENTRY(idt[8],  &exception_handler_linkage_08);
    SET_IDT_ENTRY(idt[9],  &exception_handler_linkage_09);
    SET_IDT_ENTRY(idt[10], &exception_handler_linkage_10);
    SET_IDT_ENTRY(idt[11], &exception_handler_linkage_11);
    SET_IDT_ENTRY(idt[12], &exception_handler_linkage_12);
    SET_IDT_ENTRY(idt[13], &exception_handler_linkage_13);
    SET_IDT_ENTRY(idt[14], &exception_handler_linkage_14);
    SET_IDT_ENTRY(idt[15], &exception_handler_linkage_15);
    SET_IDT_ENTRY(idt[16], &exception_handler_linkage_16);
    SET_IDT_ENTRY(idt[17], &exception_handler_linkage_17);
    SET_IDT_ENTRY(idt[18], &exception_handler_linkage_18);
    SET_IDT_ENTRY(idt[19], &exception_handler_linkage_19);
    //from handler_link
    SET_IDT_ENTRY(idt[PIT_VEC_NUM], &pit_handler_linkage);
    SET_IDT_ENTRY(idt[RTC_VEC_NUM], &rtc_handler_linkage);
	SET_IDT_ENTRY(idt[KEYBOARD_VEC_NUM], &keyboard_handler_linkage);
    SET_IDT_ENTRY(idt[SYSTEMCALL_ID], &systemcall_handler_linkage);

    // other unused entry points to default handler
    int i;
    for (i = EXCEPTION_NUM; i < IDT_NUM; i++) {
        if (i == SYSTEMCALL_ID || i == RTC_VEC_NUM || i == KEYBOARD_VEC_NUM|| i == PIT_VEC_NUM) {
            continue;
        }
        SET_IDT_ENTRY(idt[i], &idt_default_handler_linkage);
    }
    
    // initialize entry information
    for(i=0;i<IDT_NUM;i++){
        idt[i].seg_selector=KERNEL_CS;
        idt[i].size=1;
        // reserved
        // Exception and Systemcall: 01110 (0->4)
        // Interrupt: 01100 (0->4)
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 0;
        idt[i].reserved4 = 0;
        if (i < EXCEPTION_NUM || i == SYSTEMCALL_ID) {
            idt[i].reserved3 = 1;
        }
        // for system call handler, set its dpl to 3
        if(i == SYSTEMCALL_ID){
            idt[i].dpl=3;
        }
        else{
            idt[i].dpl=0;
        }
    }

    // set available entries
    for (i = 0; i < EXCEPTION_NUM; i++) {
        idt[i].present = 1;
    }

    // pit handler
    idt[PIT_VEC_NUM].present = 1;
    // rtc handler
    idt[RTC_VEC_NUM].present = 1;
    // keyboard handler
    idt[KEYBOARD_VEC_NUM].present = 1;
    // systemcall handler
    idt[SYSTEMCALL_ID].present = 1;
    
    return;
}
