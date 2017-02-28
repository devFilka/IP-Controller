#include "controller.h"

static bool is_need_on;

void controller_on(){
  is_need_on = true;
}
void controller_off(){
  is_need_on = false;
}
bool is_need_on_state(){
  return is_need_on;
}

uint8_t get_controller_status(){
  static uint8_t controller_state = CONTROLLER_START;
  switch (controller_state) {
    case CONTROLLER_START:
    case CONTROLLER_OFF:
      if( is_need_on ){
        controller_state = CONTROLLER_TURNS_ON;
      }
      break;
    case CONTROLLER_ON:
      if( !is_need_on ) {
        controller_state = CONTROLLER_TURNS_OFF;
      }
      break;
    case CONTROLLER_TURNS_ON:
    case CONTROLLER_TURNS_OFF:
      if( is_need_on ) {
        if(
            get_mb_status(true) == MB_ON &&
            get_nas_status(true) == NAS_ON &&
            get_switch_status(true) == SWITCH_ON
          ) {
          controller_state = CONTROLLER_ON;
        }
        else {
          controller_state = CONTROLLER_TURNS_ON;
        }
      }
      else {
        if(
            get_mb_status(false) == MB_OFF &&
            get_nas_status(false) == NAS_OFF &&
            get_switch_status(false) == SWITCH_OFF
          ) {
          controller_state = CONTROLLER_OFF;
        }
        else {
          controller_state = CONTROLLER_TURNS_OFF;
        }
      }
      break;
    default:
      controller_state = CONTROLLER_HZ;
      break;
  }
  if( !is_power_all() ){
    controller_state = CONTROLLER_OFF;
  }
  return controller_state;
}

void do_controller(){
  switch (get_controller_status() ){
    case CONTROLLER_TURNS_ON:
      if ( get_mb_status(true) != MB_ON ) {
        mb_power_on();
      }
      else {
        if ( get_nas_status(true) != NAS_ON ){
          nas_power_on();
        }
        else {
          switch_relay_on();
        }
      }
      break;
    case CONTROLLER_TURNS_OFF:
      if ( get_mb_status(false) != MB_OFF ) {
        mb_power_off();
      }
      if ( get_nas_status(false) != NAS_OFF ){
        nas_power_off();
      }
      if ( get_switch_status(false) != SWITCH_OFF ){
        switch_relay_off();
      }
      break;
    default :
      break;
  }
}
