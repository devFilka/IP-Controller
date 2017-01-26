#ifndef SWITCH_H
#define SWITCH_H

/* Arduino header */
#include "Arduino.h"

/* includes */
#include "time.h"

/* Switch defines */
#define SWITCH_RELAY 49
//#define SWITCH_LED 43

/* Switch constants */
#define SWITCH_RESET_DELAY 3000

/* Switch states */
#define SWITCH_START 0
#define SWITCH_ON 1
#define SWITCH_OFF 2
#define SWITCH_HZ 100

/* Switch functions */
void switch_relay_on();
void switch_relay_off();
bool switch_reset(uint32_t reset_delay);

uint8_t get_switch_status(bool is_need_on);

#endif
