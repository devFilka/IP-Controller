#include "time.h"
#include "switch.h"

static bool is_relay_on;

void switch_relay_on() {
  pinMode( SWITCH_RELAY, OUTPUT );
  digitalWrite( SWITCH_RELAY, HIGH );
  is_relay_on = true;
}
void switch_relay_off() {
  pinMode( SWITCH_RELAY, OUTPUT );
  digitalWrite( SWITCH_RELAY, LOW );
  is_relay_on = false;
}
bool switch_reset(uint32_t reset_delay){
  static uint32_t time = millis();
  if( !is_relay_on ) {
    if( is_time_end( reset_delay, time, millis()) ) {
      switch_relay_on();
      time = millis();
      return true;
    }
    else {
      switch_relay_off();
      return false;
    }
  }
  else {
    if( is_time_end( reset_delay, time, millis()) ) {
      switch_relay_on();
      time = millis();
      return true;
    }
    else {
      switch_relay_off();
      return false;
    }
  }
}

uint8_t get_switch_status(bool is_need_on){
  static uint8_t switch_state = SWITCH_START;
  switch (switch_state) {
    case SWITCH_START:
    case SWITCH_ON:
    case SWITCH_OFF:
      if( is_need_on ) {
        switch_state = SWITCH_ON;
      }
      else {
        switch_state = SWITCH_OFF;
      }
    default:
      switch_state = SWITCH_HZ;
      break;
  }
  return switch_state;
}
