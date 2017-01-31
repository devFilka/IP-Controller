#ifndef POWER_H
#define POWER_H

/* Arduino header */
#include "Arduino.h"

/* Power defines */
#define BATTERY_POWER_PIN  46
#define POWER_ALL_PIN  22

/* Power constants */

/* Power states */

/* Power functions */
void battery_off();
void battery_on();
void power_off();
void power_on();

#endif
