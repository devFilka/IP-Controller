#ifndef DCB_H
#define DCB_H

/* Arduino header */
#include "Arduino.h"

/* includes */
#include "time.h"
#include <Wire.h>
#include "../lib/PCF8574/PCF8574.h"

/* OUTF8574 defines */
#define DACB_I2C_ADDRESS    0x20  // Define I2C Address for the PCF8574AT

/* OUTF8574 pinout */
#define S1_PIN     0
#define S2_PIN     1
#define L1_GRN_PIN 2
#define L1_RED_PIN 3
#define L2_GRN_PIN 4
#define L2_RED_PIN 5
#define L3_GRN_PIN 6
#define L3_RED_PIN 7

/* led states */
#define LED_STATE_START 0
#define LED_STATE_OFF 1
#define LED_STATE_ON 2
#define LED_STATE_BLINK 3
#define LED_STATE_SHOW_MSG 4

/* led color */
#define NO_COLOR     0
#define GREEN_COLOR  1
#define RED_COLOR    2
#define ORANGE_COLOR 3

/* blink constants */
#define BLINK_BIT_DURATION 125
#define BLINK_SOME 0b11001010

/* MSG constatns */
#define MSG_STACK_SIZE 16
#define MSG_LEN 4
#define MSG_BIT_DURATION 500
#define MSG_BIT0_DURATION 100
#define MSG_BIT1_DURATION 400
#define MSG_DELAY 2000

#endif
