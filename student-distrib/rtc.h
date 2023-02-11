#ifndef _RTC_H_
#define _RTC_H_

#include "types.h"

#define RTC_ADDRESS_PORT 0x70
#define RTC_DATA_PORT 0x71
#define RTC_IRQ_NUMBER 8
#define RTC_FREQ_2 0x0F

void rtc_init();
void rtc_handler();
int32_t rtc_open(uint8_t* filename);
int32_t rtc_close(int32_t fd);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_set_virtualized_freq(int32_t freq);
int32_t test_frequency_flag;
#endif


