#ifndef RTC_H
#define RTC_H

/* Arduino header */
#include "Arduino.h"

/* includes */
#include "../lib/DS1307RTC/DS1307RTC.h"
#include "../lib/DueFlashStorage/DueFlashStorage.h"

/* Flash constatns */
#define FLASH_ADDRESS 4

/* RTC defines */

/* RTC constatns */

/* TIME struct */
struct TIME {
  tmElements_t tm;
};

/* RTC functions */
void read_cur_time();
bool getTime(const char *str);
bool getDate(const char *str);
bool is_rtc_first_setup_ok();
bool is_rtc_ok();
void setup_time();
bool get_rtc_state();
bool is_time_ok();

/* Flash functions */
TIME read_halt_time(uint32_t address);
void write_halt_time(TIME halt, uint32_t address);
void write_halt_time();
void flash_setup(TIME cur_time);

#endif
