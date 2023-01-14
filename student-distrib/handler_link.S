#define ASM 1

/* ASM_LINK
 *
 * generate as a asm linkage of handler function
 * Inputs: name,func
 * Outputs: None
 * Side Effects: generate a linkage function
 */
#define ASM_LINK(name,func)  \
.globl name                 ;\
name:                       ;\
    pushal                  ;\
    pushfl                  ;\
    call func               ;\
    popfl                   ;\
    popal                   ;\
    iret                    ;\


ASM_LINK(pit_handler_linkage         , pit_handler         );
ASM_LINK(rtc_handler_linkage         , rtc_handler         );
ASM_LINK(keyboard_handler_linkage    , keyboard_handler    );
ASM_LINK(idt_default_handler_linkage , idt_default_handler );

ASM_LINK(exception_handler_linkage_00, exception_handler_00);
ASM_LINK(exception_handler_linkage_01, exception_handler_01);
ASM_LINK(exception_handler_linkage_02, exception_handler_02);
ASM_LINK(exception_handler_linkage_03, exception_handler_03);
ASM_LINK(exception_handler_linkage_04, exception_handler_04);
ASM_LINK(exception_handler_linkage_05, exception_handler_05);
ASM_LINK(exception_handler_linkage_06, exception_handler_06);
ASM_LINK(exception_handler_linkage_07, exception_handler_07);
ASM_LINK(exception_handler_linkage_08, exception_handler_08);
ASM_LINK(exception_handler_linkage_09, exception_handler_09);
ASM_LINK(exception_handler_linkage_10, exception_handler_10);
ASM_LINK(exception_handler_linkage_11, exception_handler_11);
ASM_LINK(exception_handler_linkage_12, exception_handler_12);
ASM_LINK(exception_handler_linkage_13, exception_handler_13);
ASM_LINK(exception_handler_linkage_14, exception_handler_14);
ASM_LINK(exception_handler_linkage_15, exception_handler_15);
ASM_LINK(exception_handler_linkage_16, exception_handler_16);
ASM_LINK(exception_handler_linkage_17, exception_handler_17);
ASM_LINK(exception_handler_linkage_18, exception_handler_18);
ASM_LINK(exception_handler_linkage_19, exception_handler_19);


.globl systemcall_handler_linkage
systemcall_handler_linkage:
    pushfl
    // save all registers
    pushl   %edi
    pushl   %esi
    pushl   %ebp
    // put argument onto the stack 
    pushl   %edx
    pushl   %ecx
    pushl   %ebx
    // check whether the call number is valid
    cmp     $1, %eax
    jl      ERROR_CALL_NUMBER
    cmp     $10, %eax
    //change back to 10 because now we have 10 syscalls
    jg      ERROR_CALL_NUMBER
    call    *systemcall_table(, %eax, 4)
    jmp     systemcall_handler_linkage_return
ERROR_CALL_NUMBER:
    movl    $-1, %eax
systemcall_handler_linkage_return:
    popl    %ebx
    popl    %ecx
    popl    %edx
    popl    %ebp
    popl    %esi
    popl    %edi
    popfl
    iret


systemcall_table:
    .long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
