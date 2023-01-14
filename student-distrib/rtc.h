#ifndef RTC_H
#define RTC_H

#include "i8259.h"
#include "lib.h"

#define RTC_PORT_REG  0x70 /* 2 IO ports used for the RTC and CMOS */
#define RTC_PORT_CMOS 0x71

#define RTC_REGA 0x0A  /* rtc registers */
#define RTC_REGB 0x0B
#define RTC_REGC 0x0C

#define DIS_NMI  0x80
#define BIT_SIX  0x40  /* used for turn on bit 6 of REG B */

#define RTC_IRQ  8
#define DEFAULT_FREQ 2
#define MAX_FREQ    1024   /* Other requencies are realized by using rtc counters */
#define MAX_PROCESS 6

/* Initialize the RTC and set the rate to default value 1024Hz */
extern void rtc_init();
/* RTC handler */
extern void rtc_handler();
/* Change RTC rate based on input frequency */
extern int rtc_change_irqrate(int32_t freq);
/* Helper function for calculating rate */
extern int32_t freq_to_rate(int32_t freq);
/* Change RTC frequency to 2Hz */
extern int32_t rtc_open (const uint8_t* filenme);
/* Do nothing */
extern int32_t rtc_close (int32_t fd);
/* Wait for the next rtc interrupt */
extern int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes);
/* Change rtc freq based on the input value that buf points to */
extern int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes);
#endif
