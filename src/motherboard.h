#ifndef MB_H
#define MB_H

/* Arduino header */
#include "Arduino.h"

/* Motherboard defines */
#define MB_BTN 24
#define MB_LED 23

/* Motherboard constants */
#define MB_TURNS_ON_ERR_TIME 20000
#define MB_TURNS_OFF_ERR_TIME 120000

#define MB_ON_PRESS_BTN_DURATION 500
#define MB_ON_WAIT_BTN_DURATION 8500

#define MB_OFF_PRESS_BTN_DURATION 4000
#define MB_OFF_WAIT_BTN_DURATION 55000

/* Motherboard states */
#define MB_START 0
#define MB_ON 1
#define MB_OFF 2
#define MB_TURNS_ON 3
#define MB_TURNS_OFF 4
#define MB_TURNS_ON_ERR 5
#define MB_TURNS_OFF_ERR 6
#define MB_HZ 100

/* Motherboard functions */
void mb_btn_up();
void mb_btn_down();
void press_mb_btn(uint32_t press_ms, uint32_t wait_ms);
void mb_power_on();
void mb_power_off();
void mb_halt();
bool is_mb_led();
uint8_t get_mb_status(bool is_need_on);

#endif
