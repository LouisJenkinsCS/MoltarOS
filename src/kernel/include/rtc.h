#ifndef MOLTAROS_RTC_H
#define MOLTAROS_RTC_H

#include <stdint.h>

/*
	Driver to interface with the system's Real-Time Clock, allowing the retrieval of date and time.
*/

void rtc_init();

uint8_t rtc_get_second();

uint8_t rtc_get_minute();

uint8_t rtc_get_hour();

uint8_t rtc_get_day();

uint8_t rtc_get_month();

uint8_t rtc_get_year();

// Print time
void rtc_print();

#endif /* endif MOLTAROS_RTC_H */