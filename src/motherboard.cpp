#include "time.h"
#include "motherboard.h"

bool is_btn_pressed;

void mb_btn_up() {
  pinMode( MB_BTN, OUTPUT );
  digitalWrite( MB_BTN, HIGH );
  is_btn_pressed = false;
}
void mb_btn_down() {
  pinMode( MB_BTN, OUTPUT );
  digitalWrite( MB_BTN, LOW );
  is_btn_pressed = true;
}
void press_btn(uint32_t press_ms, uint32_t wait_ms){
  static uint32_t time = millis();
  if( is_btn_pressed ) {
    if( is_time_end( press_ms, time, millis()) ) {
      mb_btn_up();
      time = millis();
    }
    else {
      mb_btn_down();
    }
  }
  else {
    if( is_time_end( wait_ms, time, millis()) ) {
      mb_btn_down();
      time = millis();
    }
    else {
      mb_btn_up();
    }
  }
}

void mb_power_on(){
  press_btn(500, 5000);
}
void mb_power_off(){
  press_btn(1500, 20000);
}

bool is_mb_led() {
  static bool mb_led;
  pinMode( MB_LED, INPUT_PULLUP);
  mb_led = !digitalRead( MB_LED );
  return mb_led;
}

uint8_t get_mb_status(bool is_need_on){
  static uint8_t mb_state = MB_START;
  static uint32_t time = millis();
  switch (mb_state) {
    case MB_START:
    case MB_ON:
    case MB_OFF:
      if( is_mb_led() ) {
        if( is_need_on ) {
          mb_state = MB_ON;
        }
        else {
          time = millis();
          mb_state = MB_TURNS_OFF;
        }
      }
      else {
        if( is_need_on ) {
          time = millis();
          mb_state = MB_TURNS_ON;
        }
        else {
          mb_state = MB_OFF;
        }
      }
      break;
    case MB_TURNS_ON:
      if ( is_time_end( TURNS_ON_ERR_TIME, time, millis()) ) {
        mb_state = MB_TURNS_ON_ERR;
      }
      if( is_mb_led() ) {
        mb_state = MB_ON;
      }
      break;
    case MB_TURNS_OFF:
      if( is_time_end( TURNS_OFF_ERR_TIME, time, millis()) ) {
        mb_state = MB_TURNS_OFF_ERR;
      }
      if( !is_mb_led() ) {
        mb_state = MB_OFF;
      }
      break;
    case MB_TURNS_ON_ERR:
    case MB_TURNS_OFF_ERR:
      if( is_mb_led() ) {
        if( is_need_on ) {
          mb_state = MB_ON;
        }
      }
      else {
        if( !is_need_on ) {
          mb_state = MB_OFF;
        }
      }
      break;
    default:
      mb_state = MB_HZ;
      break;
  }
  return mb_state;
}
