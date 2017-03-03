#ifndef FAN_H
#define FAN_H

/* Arduino header */
#include "Arduino.h"

/* Fan defines */
#define FAN_1_PIN 2
#define FAN_2_PIN 13

/* Fan constants */
#define MIN_FAN_PWM 0
#define MAX_FAN_PWM 255

/* Fan functions */
void set_fans( uint8_t percent );
void fans_max();
void fans_off();

#endif
