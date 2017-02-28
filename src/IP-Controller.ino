#include "dcb.h"
#include "fan.h"
#include "time.h"

#include "voltage.h"
#include "power.h"
#include "rtc.h"

#include "motherboard.h"
#include "nas.h"
#include "switch.h"
#include "controller.h"

#include "ascii.h"

/* states */
bool is_voltage_norm = false;
bool is_battery_time_ok = false;
bool is_some_error = false;
bool is_halt = false;
bool halt_wait = false;
uint32_t halt_wait_time;

/* leds */
Led L1 = { L1_GRN_PIN, L1_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2_state = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2_battery = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2_button = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L3 = { L3_GRN_PIN, L3_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };

void led_blink(Led *led, uint8_t color, uint32_t blink_ms, uint32_t blink_wait_ms, bool one);
void setup_leds(){
  clear_msg_stack(&L1);
  clear_msg_stack(&L2_state);
  clear_msg_stack(&L2_battery);
  clear_msg_stack(&L2_button);
  clear_msg_stack(&L3);

  L1.blink_state = BLINK_STATE_START;
  L2_state.blink_state = BLINK_STATE_START;
  L2_battery.blink_state = BLINK_STATE_START;
  L2_button.blink_state = BLINK_STATE_START;
  L3.blink_state = BLINK_STATE_START;

  led_blink(&L2_state, GREEN_COLOR, 1, 0, true);
  led_blink(&L2_battery, GREEN_COLOR, 1, 0, true);
  led_blink(&L2_button, GREEN_COLOR, 1, 0, true);
}
void do_leds(){
  do_led(&L1);
  do_led(&L2_state);
  do_led(&L2_battery);
  do_led(&L2_button);
  do_led(&L3);
}
void show_current_state(uint8_t msg){
  if( !is_some_error ){
    if( is_stack_clear(&L1) ) {
      add_msg(&L1, msg);
      L1.color = GREEN_COLOR;
      L1.state = LED_STATE_SHOW_MSG;
    }
  }
}
void show_current_error(uint8_t msg){
  is_some_error = true;
  add_msg(&L1, msg);
  L1.color = RED_COLOR;
  L1.state = LED_STATE_SHOW_MSG;
}
void show_controller_error(){
  is_some_error = false;
  if( get_controller_status() != CONTROLLER_OFF ){
    if( get_mb_status(is_need_on_state()) == MB_TURNS_ON_ERR ) {
      show_current_error(1);
    }
    if( get_mb_status(is_need_on_state()) == MB_TURNS_OFF_ERR ) {
      show_current_error(2);
    }
    if( get_nas_status(is_need_on_state()) == NAS_TURNS_ON_ERR ) {
      show_current_error(3);
    }
    if( get_nas_status(is_need_on_state()) == NAS_TURNS_OFF_ERR ) {
      show_current_error(4);
    }
  }
}
void led_on_with_color(Led *led, uint8_t color){
  led->color = color;
  led->state = LED_STATE_ON;
}
void led_off_no_color(Led *led){
  led->color - NO_COLOR;
  led->state = LED_STATE_OFF;
}
void led_blink(Led *led, uint8_t color, uint32_t blink_ms, uint32_t blink_wait_ms, bool one){
  if(one){
    led->state = LED_STATE_BLINK_ONE;
  }
  else{
    led->state = LED_STATE_BLINK;
  }
  led->color = color;
  led->blink_ms = blink_ms;
  led->blink_wait_ms = blink_wait_ms;
}

/* buttons */
Button S1 = { S1_PIN };
Button S2 = { S2_PIN };

/* i2c */
bool is_i2c_error(uint8_t address){
  Wire.beginTransmission(address);
  if( Wire.endTransmission() == 0 ){
    return false;
  }
  else {
    return true;
  }
}
bool check_i2c(){
  if( is_i2c_error(0x20) || is_i2c_error(0x68) ){
    Wire.begin();
    setup_dcb();
  }
}

uint32_t critical_halt_time_counter;

void setup(){
  battery_off();
  power_off();
  controller_off();

  fans_max();

  setup_leds();
  setup_dcb();
  setup_spi();
  setup_time();

  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

  led_on_with_color(&L3, RED_COLOR);
  show_current_state(0x00);
}

#define HALT_STATE_NO_HALT 0
#define HALT_STATE_HALT_NOW 1
#define HALT_STATE_WAIT 2
#define HALT_STATE_NO_WAIT 3
uint8_t halt_state = HALT_STATE_NO_HALT;
uint32_t halt_time_counter = millis();
uint32_t halt_show_time_counter = millis();
bool halt_show_color = true;

#define CONTROLLER_ON_STATE_OFF 0
#define CONTROLLER_ON_STATE_BATTERY 1
#define CONTROLLER_ON_STATE_POWER 2
#define CONTROLLER_ON_STATE_ALL 3
#define CONTROLLER_ON_DELAY 1000
uint8_t controller_on_state = CONTROLLER_ON_STATE_OFF;
uint32_t controller_on_time_counter = millis();


void loop(){

  if( is_start_voltage() ){
    if( halt_state != HALT_STATE_WAIT ){
      led_on_with_color(&L3, GREEN_COLOR);
    }
    is_voltage_norm = true;
  }
  else if( is_stop_voltage() ) {
    if( halt_state != HALT_STATE_WAIT ){
      led_on_with_color(&L3, RED_COLOR);
    }
    is_voltage_norm = false;
  }

  if( is_time_ok() ){
    is_battery_time_ok = true;
    L2_battery.state = LED_STATE_PAUSE;
  }
  else {
    is_battery_time_ok = false;
    led_blink(&L2_battery, RED_COLOR, 100, 3000, false);
  }

  switch ( get_controller_status() ){
    case CONTROLLER_OFF:
      power_off();
      battery_off();
      show_current_state(0x01);
      led_off_no_color(&L2_state);
      if( is_voltage_norm ){
        if( is_button_pressed(&S1) ){
          battery_on();
          power_on();
          controller_on();
          led_blink(&L2_button, GREEN_COLOR, 100, 0, true);
        }
        if( is_button_pressed(&S2) ){
          led_blink(&L2_button, RED_COLOR, 1000, 0, true);
        }
      }
      else {
        if( is_button_pressed(&S1) || is_button_pressed(&S2) ){
          led_blink(&L2_button, RED_COLOR, 1000, 0, true);
        }
      }
      break;
    case CONTROLLER_ON:
      led_on_with_color(&L2_state, GREEN_COLOR);
      show_current_state(0x0f);
      if( is_button_pressed(&S2) ){
        controller_off();
        led_blink(&L2_button, GREEN_COLOR, 100, 0, true);
      }
      else if( is_button_pressed(&S1) ){
        led_blink(&L2_button, RED_COLOR, 1000, 0, true);
      }
      break;
    case CONTROLLER_TURNS_ON:
      show_current_state(0x03);
      led_blink(&L2_state, GREEN_COLOR, 500, 500, false);
      if( is_button_pressed(&S1) ){
        led_blink(&L2_button, GREEN_COLOR, 1000, 0, true);
      }
      else if( is_button_pressed(&S2) ){
        led_blink(&L2_button, RED_COLOR, 1000, 0, true);
      }
      break;
    case CONTROLLER_TURNS_OFF:
      show_current_state(0x07);
      led_blink(&L2_state, GREEN_COLOR, 1000, 1000, false);
      if( is_button_pressed(&S2) ){
        led_blink(&L2_button, GREEN_COLOR, 1000, 0, true);
      }
      else if( is_button_pressed(&S1) ){
        led_blink(&L2_button, RED_COLOR, 1000, 0, true);
      }
      break;
    default:
      break;
  }

  switch(halt_state){
    case HALT_STATE_NO_HALT:
      if( !is_voltage_norm ){
        if( get_controller_status() != CONTROLLER_OFF ){
          controller_off();
          halt_state = HALT_STATE_HALT_NOW;
        }
      }
      break;
    case HALT_STATE_HALT_NOW:
      write_halt_time();
      if( is_battery_time_ok ){
        halt_time_counter = millis();
        halt_state = HALT_STATE_WAIT;
      }
      else {
        power_off();
        battery_off();
        halt_state = HALT_STATE_NO_HALT;
      }
      break;
    case HALT_STATE_WAIT:
      if( is_time_end(1000, halt_show_time_counter, millis()) ){
        halt_show_time_counter = millis();
        if( halt_show_color ) {
          halt_show_color = false;
          led_on_with_color(&L3, GREEN_COLOR);
        }
        else {
          halt_show_color = true;
          led_on_with_color(&L3, RED_COLOR);
        }
      }
      show_current_state(0x07);
      if( is_time_end(120000, halt_time_counter, millis()) ){
        power_off();
        battery_off();
        halt_state = HALT_STATE_NO_HALT;
      }
      break;
    default:
      break;
  }

  get_voltage_res();

  do_controller();
  do_leds();

  show_controller_error();

  check_i2c();

  if( is_button_pressed(&S2) ){
    if( is_time_end(10000, critical_halt_time_counter, millis())){
      controller_off();
      power_off();
      battery_off();
    }
  }
  else {
    critical_halt_time_counter = millis();
  }
}
