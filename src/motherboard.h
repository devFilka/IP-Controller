#ifndef MB_H
#define MB_H

#include "Arduino.h"

#define MB_BTN 23
#define MB_LED 24

#define MAX_MILLIS 4294967295

#define TURNS_ON_ERR_TIME 10000
#define TURNS_OFF_ERR_TIME 50000

#define MB_START 0
#define MB_ON 1
#define MB_OFF 2
#define MB_TURNS_ON 3
#define MB_TURNS_OFF 4
#define MB_TURNS_ON_ERR 5
#define MB_TURNS_OFF_ERR 6
#define MB_HZ 100

void mb_btn_up();
void mb_btn_down();
bool is_mb_led();
uint8_t get_mb_status(bool is_need_on);

bool is_time_end(uint32_t t, uint32_t begin, uint32_t end = millis());

#endif
