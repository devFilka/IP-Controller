#ifndef DCB_H
#define DCB_H

/* Arduino header */
#include "Arduino.h"

/* includes */
//#include "time.h"
#include <Wire.h>
#include "../lib/PCF8574/PCF8574.h"


/* OUTF8574 defines */
#define DACB_I2C_ADDRESS    0x20  // Define I2C Address for the PCF8574AT

/* I2C RESET */
#define I2C_RESET_PIN 62
#define I2C_RESET_DELAY 20000

void dacb_i2c_off();
void dacb_i2c_on();

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
#define LED_STATE_SHOW_MSG 3
#define LED_STATE_BLINK 4
#define LED_STATE_BLINK_ONE 5
#define LED_STATE_PAUSE 6

/* led color */
#define NO_COLOR     0
#define GREEN_COLOR  1
#define RED_COLOR    2
#define ORANGE_COLOR 3

/* MSG constatns */
#define MSG_STACK_SIZE 16
#define MSG_LEN 4
#define MSG_BIT_DURATION 500
#define MSG_BIT0_DURATION 100
#define MSG_BIT1_DURATION 400
#define MSG_DELAY_DURATION 2000

/* MSG states */
#define MSG_STATE_START 0
#define MSG_STATE_READ_MSG 1
#define MSG_STATE_READ_BIT 2
#define MSG_STATE_0 3
#define MSG_STATE_1 4
#define MSG_STATE_END_BIT 5
#define MSG_STATE_DELAY 6

/* blink states */
#define BLINK_STATE_START 0
#define BLINK_STATE_BLINK 1
#define BLINK_STATE_WAIT 2

/* led struct */
struct Led {
  const uint8_t green_pin;
  const uint8_t red_pin;
  uint8_t state;
  uint8_t msg_state;
  uint32_t time_counter;
  uint8_t color;
  uint8_t msg_stack[MSG_STACK_SIZE];
  uint8_t cur_msg;
  uint8_t cur_msg_bit;
  uint8_t write_point;
  uint8_t read_point;
  uint8_t blink_state;
  uint32_t blink_ms;
  uint32_t blink_wait_ms;
};

/* led functions */
void setup_dcb();
void led_off(Led *led);
void led_on(Led *led);
void add_msg(Led *led, uint8_t msg);
void clear_msg_stack(Led *led);
bool is_stack_clear(Led *led);
void led_show_msg(Led *led);
void led_blink(Led *led);
void led_blink_one(Led *led);
void do_led(Led *led);
bool is_dcb_i2c_err();

/* button constatns */
#define BTN_STACK_SIZE 50

/* button struct */
struct Button {
  const uint8_t btn_pin;
  bool btn_stack[BTN_STACK_SIZE];
  uint8_t btn_point;
};

/* button functions */
bool is_button_pressed(Button *btn);

#endif
