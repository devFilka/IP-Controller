#include "uart.h"
#include "motherboard.h"
#include "controller.h"
#include "nas.h"
#include "switch.h"
#include "time.h"
#include "power.h"
#include "dcb.h"
#include "fan.h"

Led L1 = { L1_GRN_PIN, L1_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2 = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L3 = { L3_GRN_PIN, L3_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };

Button S1 = { S1_PIN };
Button S2 = { S2_PIN };

/* setup */
void setup(){
  
}
void loop(){

}
