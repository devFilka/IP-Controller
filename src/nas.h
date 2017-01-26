#ifndef NAS_H
#define NAS_H

/* Arduino header */
#include "Arduino.h"

/* includes */
#include "time.h"

/* NAS defines */
#define NAS_BTN 42
#define NAS_LED 71

/* NAS constants */
#define NAS_TURNS_ON_ERR_TIME 240000
#define NAS_TURNS_OFF_ERR_TIME 120000

#define NAS_ON_PRESS_BTN_DURATION 2000
#define NAS_ON_WAIT_BTN_DURATION 117000

#define NAS_OFF_PRESS_BTN_DURATION 4000
#define NAS_OFF_WAIT_BTN_DURATION 55000

/* NAS states */
#define NAS_START 0
#define NAS_ON 1
#define NAS_OFF 2
#define NAS_TURNS_ON 3
#define NAS_TURNS_OFF 4
#define NAS_TURNS_ON_ERR 5
#define NAS_TURNS_OFF_ERR 6
#define NAS_HZ 100

/* NAS functions */
void nas_btn_up();
void nas_btn_down();
void press_nas_btn(uint32_t press_ms, uint32_t wait_ms);
void nas_power_on();
void nas_power_off();
void nas_halt();
bool is_nas_led();
uint8_t get_nas_status(bool is_need_on);

#endif
