#include "system_call.h"
#include "terminal.h"

/* Reference: https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer */
#define PIT_IRQ             0
#define PIT_CHANNEL0        0x40  //used as system timer by OS
// channel1 & 2 ommited here
#define PIT_COMMAND_PORT    0x43
/* For Mode/Command register at command port
   bit 6&7: 00 for channel 0
   bit 4&5: 11 for both lo/hibyte access mode
   bit 1-3: 011 Mode3 for square wave generator
   bit 0:   0 for binary mode operation
 */
#define PIT_MODE_REG        0x36  //00110110b

/* For PIT freq calculations 
   PIT signal frequency: 1193182Hz
   divisor = 1193182/desired frequency
   Assign 10ms for each time slices, 100Hz is desired
 */
#define PIT_FREQ_DIVISOR    11932   //1193182Hz/100Hz
#define HIGH_BYTE_MASK      0xFF
#define HIGH_BITS_SHIFT     8

#define terminal_total_num  3

#define PROGRAM_IMAGE       0x08000000  //start at 128MB


extern volatile int32_t shell_tid ;
/* initialize PIT */
extern void init_PIT();

/* PIT interrupt */
extern void pit_handler();

/* process switch */
void process_switch(int32_t next_process);

/* reschedule */
int32_t reschedule();












