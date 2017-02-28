#include "switch.h"

static bool is_relay_on;

void switch_relay_on() {
  pinMode( SWITCH_RELAY, OUTPUT );
  digitalWrite( SWITCH_RELAY, LOW );
  is_relay_on = true;
}
void switch_relay_off() {
  pinMode( SWITCH_RELAY, OUTPUT );
  digitalWrite( SWITCH_RELAY, HIGH );
  is_relay_on = false;
}

uint8_t get_switch_status(bool is_need_on){
  static uint8_t switch_state = SWITCH_START;
  switch (switch_state) {
    case SWITCH_START:
    case SWITCH_OFF:
    case SWITCH_ON:
    case SWITCH_TURNS_OFF:
    case SWITCH_TURNS_ON:
      if( is_need_on ) {
        if( is_relay_on ){
          switch_state = SWITCH_ON;
        }
        else {
          switch_state = SWITCH_TURNS_ON;
        }
      }
      else {
        if( !is_relay_on ){
          switch_state = SWITCH_OFF;
        }
        else {
          switch_state = SWITCH_TURNS_OFF;
        }
      }
      break;
    default:
      switch_state = SWITCH_HZ;
      break;
  }
  return switch_state;
}





































/*
uint8_t get_mb_status(bool is_need_on){
  static uint8_t mb_state = MB_START;
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
      if( is_mb_led() ) {
        mb_state = MB_ON;
      }
      break;
    case MB_TURNS_OFF:
      if( !is_mb_led() ) {
        mb_state = MB_OFF;
      }
      break;
    default:
      mb_state = MB_HZ;
      break;
  }
  return mb_state;
}
*/
