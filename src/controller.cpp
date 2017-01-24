#include "controller.h"

#include "motherboard.h"
#include "switch.h"
#include "nas.h"

static bool is_need_on;

void controller_on(){
  is_need_on = true;
}
void controller_off(){
  is_need_on = false;
}

uint8_t get_controller_status(){
  static uint8_t controller_state = CONTROLLER_START;
  static uint32_t time = millis();
  switch (controller_state) {
    case CONTROLLER_START:
    case CONTROLLER_ON:
    case CONTROLLER_OFF:
    case CONTROLLER_TURNS_ON:
    case CONTROLLER_TURNS_OFF:
      if( is_need_on ) {
        if(
            get_mb_status(is_need_on) == MB_ON &&
            get_nas_status(is_need_on) == NAS_ON &&
            get_switch_status(is_need_on) == SWITCH_ON
          ) {
          controller_state = CONTROLLER_ON;
        }
        else {
          controller_state = CONTROLLER_TURNS_ON;
        }
      }
      else {
        if(
            get_mb_status(is_need_on) == MB_OFF &&
            get_nas_status(is_need_on) == NAS_OFF &&
            get_switch_status(is_need_on) == SWITCH_OFF
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
  return controller_state;
}
