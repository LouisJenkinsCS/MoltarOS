#include <include/rtc.h>
#include <include/io_port.h>
#include <stdio.h>
#include <stdbool.h>

// Output port for Real-Time Clock
#define RTC_OUT 0x70
// Input port for Real-Time Clock
#define RTC_IN 0x71

static uint8_t as_binary(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rtc_init() {
	// TODO
}

uint8_t rtc_get_second() {
	outb(RTC_OUT, 0x00);
	return as_binary(inb(RTC_IN));
}

uint8_t rtc_get_minute() {
	outb(RTC_OUT, 0x02);
	return as_binary(inb(RTC_IN));
}

uint8_t rtc_get_hour() {
	outb(RTC_OUT, 0x04);
	return as_binary(inb(RTC_IN));
}

uint8_t rtc_get_day() {
	outb(RTC_OUT, 0x07);
	return as_binary(inb(RTC_IN));
}

uint8_t rtc_get_month() {
	outb(RTC_OUT, 0x08);
	return as_binary(inb(RTC_IN));
}

uint8_t rtc_get_year() {
	outb(RTC_OUT, 0x09);
	return as_binary(inb(RTC_IN));
}

// Print time
void rtc_print() {
	// EST is UTC-5
	int16_t hour = rtc_get_hour();
	bool pm = false;
	hour = (hour - 5) % 24;

	if(hour > 12) {
		pm = true;
		hour -= 12;
	}

	if(hour < 0)
		hour += 12;

	printf("%d:%d:%d %s", hour, rtc_get_minute(), rtc_get_second(), pm ? "PM" : "AM");
}