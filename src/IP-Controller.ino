#include "dcb.h"
#include "fan.h"

#include "voltage.h"
#include "power.h"
#include "rtc.h"

#include "motherboard.h"
#include "nas.h"
#include "switch.h"
#include "controller.h"

/* states */
bool is_can_be_on = false;
bool is_some_error = false;
bool is_halt = false;

/* leds */
Led L1 = { L1_GRN_PIN, L1_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2 = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L3 = { L3_GRN_PIN, L3_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };

void setup_leds(){
  clear_msg_stack(&L1);
  clear_msg_stack(&L2);
  clear_msg_stack(&L3);
}
void do_leds(){
  do_led(&L1);
  do_led(&L2);
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
void led_on_with_color(Led *led, uint8_t color){
  led->color = color;
  led->state = LED_STATE_ON;
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

  led_on_with_color(&L3, RED_COLOR);
  show_current_state(0x00);
}

void loop(){
  if( is_start_voltage() ) {
    led_on_with_color(&L3, GREEN_COLOR);
    if( is_power_all() ) {
      if( get_controller_status() == CONTROLLER_ON ) {
        if( is_button_pressed(&S2) ) {
          is_halt = false;
          controller_off();
        }
        else { }
      }
      else {
        show_controller_error();
      }
    }
    else {
      if( is_button_pressed(&S1) ) {
        controller_on();
      }
      else { }
    }
  }
  else {
    if( is_stop_voltage() ) {
      led_on_with_color(&L3, RED_COLOR);
      is_halt = true;
      controller_off();
    }
    else {
      if( is_power_all() ) {
        if( get_controller_status() == CONTROLLER_OFF ) {
          power_off();
        }
        else { }
      }
    }
  }















  get_voltage_res();

  do_controller();
  do_leds();

  check_i2c();









  Serial.print(" | ALL :");
  Serial.print(get_controller_status());
  Serial.print(" | MB :");
  Serial.print(get_mb_status(is_need_on_state()));
  Serial.print(" | SW :");
  Serial.print(get_switch_status(is_need_on_state()));
  Serial.print(" | NAS :");
  Serial.print(get_nas_status(is_need_on_state()));
  Serial.println(" | ");

}





















/*
void loop(){
  is_some_error = false;

  if( is_start_voltage() ) {
    battery_on();
    power_on();
  }
  else {
    if( is_button_pressed(&S1) ){
      show_current_error(0x04);
    }
    else if( is_button_pressed(&S2) ){
      show_current_error(0x05);
    }
    battery_off();
    power_off();
  }

  show_current_state(0x01);

  set_fans(0);
  set_fans(33);
  set_fans(67);
  set_fans(100);

  static uint32_t voltage = get_current_voltage_res();
  if( voltage != get_current_voltage_res() ){
    voltage = get_current_voltage_res();
    Serial.print("V = ");
    Serial.println(voltage);
  }

  static bool i2c_err = true;
  if( is_i2c_error(0x20) || is_i2c_error(0x68) ){
    if( !i2c_err ){
      i2c_err = true;
      Serial.println("I2C ERROR!");
    }
    Wire.begin();
    setup_dcb();
  }
  else {
    if( i2c_err ){
      i2c_err = false;
      Serial.println("I2C OK!");
    }
  }

  get_voltage_res();
  do_leds();
}
*/
