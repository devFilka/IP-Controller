#ifndef CONTROLLER_H
#define CONTROLLER_H

/* Arduino header */
#include "Arduino.h"

/* Controller constants */
#define POWER_OFF_PIN 22

/* Controller states */
#define CONTROLLER_START 0
#define CONTROLLER_ON 1
#define CONTROLLER_OFF 2
#define CONTROLLER_TURNS_ON 3
#define CONTROLLER_TURNS_OFF 4
#define CONTROLLER_HZ 100

/*  Controller error states*/
#define CONTROLLER_NO_ERROR 0
#define CONTROLLER_MB_ERROR 1
#define CONTROLLER_SWITCH_ERROR 2
#define CONTROLLER_NAS_ERROR 4

/* Controller functions */;
void controller_on();
void controller_off();
uint8_t get_controller_status();

#endif
