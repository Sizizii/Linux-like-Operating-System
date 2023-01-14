#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define ONEK 1024
#define FOURK 4096
// four megabyte
#define FOURM 0x400000
#define VIDEO 0xB8000
#define PRESENT 0x01
#define READ    0x02
#define READ_P  0x03
#define USER    0x04
#define ACCESS  0x20
#define DIRTY   0x40
#define MSIZE   0x80
#define _4MB    0x00400000
#define _4KB    0x00001000
#define _160MB_idx  40
#define _164MB_idx  41
#define terminal0_buffer    0xD0000 
#define terminal1_buffer    0xD1000
#define terminal2_buffer    0xD2000
// set up the paging structure
uint32_t page_directory[ONEK] __attribute__ ((aligned(FOURK)));
uint32_t page_table[ONEK] __attribute__ ((aligned(FOURK)));
uint32_t vid_page_table[ONEK] __attribute__ ((aligned(FOURK)));
/* Initialize paging */
void paging_init(void);

/* load the page directory address into cr3 */
extern void loadPageDirectory(unsigned int*);

/* first set the cr4 lowest second bit to enable 4mb page, then set the highest bit of cr0 to enable paging*/
extern void Enable_paging();

/* flush the tlb whenever we change the page directory*/
extern void Flush_tlb();

/* load program to physical memory */
extern void map_process(uint32_t virtual_MEM, uint32_t physical_MEM);
/* map the page directory entry to our program video part */
extern void vid_mapping();


extern void terminal_mapping();
/* set up the physical video mem to the virtual mem */
extern void vid_remapping(uint32_t physical_MEM);
#endif
