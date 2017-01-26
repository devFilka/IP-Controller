#include "nas.h"

static bool is_btn_pressed;

void nas_btn_up() {
  pinMode( NAS_BTN, OUTPUT );
  digitalWrite( NAS_BTN, HIGH );
  is_btn_pressed = false;
}
void nas_btn_down() {
  pinMode( NAS_BTN, OUTPUT );
  digitalWrite( NAS_BTN, LOW );
  is_btn_pressed = true;
}
void press_nas_btn(uint32_t press_ms, uint32_t wait_ms){
  static uint32_t time = millis();
  if( is_btn_pressed ) {
    if( is_time_end( press_ms, time, millis()) ) {
      nas_btn_up();
      time = millis();
    }
    else {
      nas_btn_down();
    }
  }
  else {
    if( is_time_end( wait_ms, time, millis()) ) {
      nas_btn_down();
      time = millis();
    }
    else {
      nas_btn_up();
    }
  }
}

void nas_power_on(){
  if( !is_nas_led() ){
    press_nas_btn(NAS_ON_PRESS_BTN_DURATION, NAS_ON_WAIT_BTN_DURATION);
  }
  else {
    nas_btn_up();
  }
}
void nas_power_off(){
    if( is_nas_led() ){
      press_nas_btn(NAS_OFF_PRESS_BTN_DURATION, NAS_OFF_WAIT_BTN_DURATION);
    }
    else {
      nas_btn_up();
    }
}
void nas_halt(){
  if( is_nas_led() ) {
    nas_btn_down();
  }
  else {
    nas_btn_up();
  }
}

bool is_nas_led() {
  static bool nas_led;
  pinMode( NAS_LED, INPUT_PULLUP);
  nas_led = !digitalRead( NAS_LED );
  return nas_led;
}

uint8_t get_nas_status(bool is_need_on){
  static uint8_t nas_state = NAS_START;
  static uint32_t time = millis();
  switch (nas_state) {
    case NAS_START:
    case NAS_ON:
    case NAS_OFF:
      if( is_nas_led() ) {
        if( is_need_on ) {
          nas_state = NAS_ON;
        }
        else {
          time = millis();
          nas_state = NAS_TURNS_OFF;
        }
      }
      else {
        if( is_need_on ) {
          time = millis();
          nas_state = NAS_TURNS_ON;
        }
        else {
          nas_state = NAS_OFF;
        }
      }
      break;
    case NAS_TURNS_ON:
      if ( is_time_end( NAS_TURNS_ON_ERR_TIME, time, millis()) ) {
        nas_state = NAS_TURNS_ON_ERR;
      }
      if( is_nas_led() ) {
        nas_state = NAS_ON;
      }
      break;
    case NAS_TURNS_OFF:
      if( is_time_end( NAS_TURNS_OFF_ERR_TIME, time, millis()) ) {
        nas_state = NAS_TURNS_OFF_ERR;
      }
      if( !is_nas_led() ) {
        nas_state = NAS_OFF;
      }
      break;
    case NAS_TURNS_ON_ERR:
    case NAS_TURNS_OFF_ERR:
      if( is_nas_led() ) {
        if( is_need_on ) {
          nas_state = NAS_ON;
        }
      }
      else {
        if( !is_need_on ) {
          nas_state = NAS_OFF;
        }
      }
      break;
    default:
      nas_state = NAS_HZ;
      break;
  }
  return nas_state;
}
