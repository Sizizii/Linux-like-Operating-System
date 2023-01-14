/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define MASTER_8259_DATA    (MASTER_8259_PORT + 1)
#define SLAVE_8259_DATA     (SLAVE_8259_PORT + 1)
#define MASK_ALL            0xFF
#define MASK_0              0xFE
/* masked interupts are enabled */
uint8_t master_mask = MASK_ALL; /* IRQs 0-7  */
uint8_t slave_mask = MASK_ALL;  /* IRQs 8-15 */

/* i8259_init: Initialize the 8259 PIC
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: Initialize the 8259 PIC
 */
void i8259_init(void) {
    outb(ICW1,MASTER_8259_PORT); // MASK THE TWO SPECIAL INTERRUPT THE MASTER PIC
    outb(ICW2_MASTER,MASTER_8259_DATA); // map master ir0-7 to 0x20-0x27    
    outb(ICW3_MASTER,MASTER_8259_DATA); // the primary pic has a secondary on IRQ2
    outb(ICW4,MASTER_8259_DATA);//icw4

    outb(ICW1,SLAVE_8259_PORT); // MASK THE TWO SPECIAL INTERRUPT THE SLAVE PIC
    outb(ICW2_SLAVE,SLAVE_8259_DATA);   // map slave ir0-7 to 0x28-0x2f
    outb(ICW3_SLAVE, SLAVE_8259_DATA); 
    outb(ICW4,SLAVE_8259_DATA);//icw4 Set mode for pics
    enable_irq(irq_slave_on_master);
}


/* enable_irq: Enable (unmask) the specified IRQ
 * Input: none
 * Output: none
 * return: none
 * side_effect: change mask of pic
 */
void enable_irq(uint32_t irq_num) {
    uint8_t mask = MASK_0; //0xFE means 11111110, only the last port is 0
    int i;
    if (irq_num > irq_total || irq_num < 0) { 
 		return; 
 	}

    if (irq_num < irq_half) {
        for (i = 0; i < irq_num; i++) {
            mask = (mask << 1) + 1;
        }
        master_mask &= mask;
        outb(master_mask, MASTER_8259_DATA); //this is the master pic mask change
        return;
    }

    // irq number >= 8
    irq_num -= irq_half;
    for (i = 0; i < irq_num; i++) {
        mask = (mask << 1) + 1;
    }
    slave_mask &= mask;    
    outb(slave_mask, SLAVE_8259_DATA);//this is the slave pic mask change
    return;
}

/* disable_irq: Disable (mask) the specified IRQ
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: change mask of pic
 */
void disable_irq(uint32_t irq_num) {
    uint8_t mask = 0x01; // only mask the last bit
    int i;
    if (irq_num > irq_total || irq_num < 0) { 
 		return; 
 	}

    if (irq_num < irq_half) {  //check if this is master irq
        for (i = 0; i < irq_num; i++) {
            mask <<= 1; 
        }
        master_mask |= mask;
        outb(master_mask, MASTER_8259_DATA);
        return;
    }

    // irq number >= 8
    irq_num -= irq_half;
    for (i = 0; i < irq_num; i++) {
        mask <<= 1;
    }
    slave_mask |= mask;    
    outb(slave_mask, SLAVE_8259_DATA);
    return;
}

/* send_eoi: Send end-of-interrupt signal for the specified IRQ
 * 
 * Input: none
 * Output: none
 * return: none
 * side_effect: send signal to pic
 */
void send_eoi(uint32_t irq_num) {
	/* MASTER IRQ : 0 -> 7 */
	if (irq_num >= 0 && irq_num < irq_half) { 
 		outb( EOI | irq_num, MASTER_8259_PORT); 
 	}
 
 	/* SLAVE IRQ : 8 -> 15*/
 	if (irq_num >= irq_half && irq_num <= irq_total) { 
 		outb( EOI | (irq_num - irq_half), SLAVE_8259_PORT); 
 		outb( EOI | irq_slave_on_master, MASTER_8259_PORT); 
 	}
    return;
}
