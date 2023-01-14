/* loadPageDirectory
 *
 * find page directory by putting it's address into the cr3 register.
 * Inputs: None
 * Outputs: None
 * Side Effects: change the value of cr3
 * Coverage: paging
 */
.global loadPageDirectory
loadPageDirectory:
pushl %ebp
movl %esp, %ebp
movl 8(%esp), %eax
movl %eax, %cr3
movl %ebp, %esp
popl %ebp
ret

/* Enable_paging
 *
 * 
 * We set the 32th bit in the CR0 register and the lowest bit of the cr0
 * set up the value for cr4 to enable the 4Mb page
 * Inputs: None
 * Outputs: None
 * Side Effects: change the value of the register
 * Coverage: paging
 */
.global Enable_paging
Enable_paging:
pushl %ebp
movl %esp, %ebp
movl %cr4, %eax
orl  $0x00000010, %eax
movl %eax, %cr4
movl %cr0, %eax
orl  $0x80000000, %eax
movl %eax, %cr0
movl %ebp, %esp
popl %ebp
ret

/*
 * Flush_tlb
 *
 * remap the page directory to make sure the
 * mapping is correct when we try to switch the process
 * Input: none
 * Output: tlb is remapped
 * Return value: none
 * Side_effect: the tlb is renewed
 *
*/
.global Flush_tlb
Flush_tlb:
pushl %ebp
movl  %esp, %ebp
movl  %cr3, %eax
movl  %eax, %cr3
movl  %ebp, %esp
popl  %ebp
ret










