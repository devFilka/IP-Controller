#include "dcb.h"

PCF8574 DCB;

/* WTF? WHY THIS NOT FOUND IN time.h? */
#define MAX_MILLIS 4294967295

bool is_time_end(uint32_t t, uint32_t begin, uint32_t end) {
  if( end >= begin ){
    if( (end-begin) >= t ){
      return true;
    }
    else {
      return false;
    }
  }
  else {
    if( (MAX_MILLIS-begin+end) >= t ){
      return true;
    }
    else {
      return false;
    }
  }
}

void setup_dcb() {
  DCB.begin(DACB_I2C_ADDRESS);

  DCB.pinMode(S1_PIN, INPUT_PULLUP);
  DCB.pinMode(S2_PIN, INPUT_PULLUP);

  DCB.pinMode(L1_GRN_PIN, OUTPUT);
  DCB.digitalWrite(L1_GRN_PIN, HIGH);
  DCB.pinMode(L1_RED_PIN, OUTPUT);
  DCB.digitalWrite(L1_RED_PIN, HIGH);

  DCB.pinMode(L2_GRN_PIN, OUTPUT);
  DCB.digitalWrite(L2_GRN_PIN, HIGH);
  DCB.pinMode(L2_RED_PIN, OUTPUT);
  DCB.digitalWrite(L2_RED_PIN, HIGH);

  DCB.pinMode(L3_GRN_PIN, OUTPUT);
  DCB.digitalWrite(L3_GRN_PIN, HIGH);
  DCB.pinMode(L3_RED_PIN, OUTPUT);
  DCB.digitalWrite(L3_RED_PIN, HIGH);
}

/* led functions */
void led_off(Led *led){
  DCB.digitalWrite(led->green_pin, HIGH);
  DCB.digitalWrite(led->red_pin, HIGH);
}
void led_on(Led *led){
  led->color & ( 1 << 0 ) ? DCB.digitalWrite(led->green_pin, LOW) : DCB.digitalWrite(led->green_pin, HIGH);
  led->color & ( 1 << 1 ) ? DCB.digitalWrite(led->red_pin, LOW)  : DCB.digitalWrite(led->red_pin, HIGH);
}
void add_msg(Led *led, uint8_t msg){
  uint8_t write_point = led->write_point;
  uint8_t i;
  bool is_need_add_msg;
  for ( i = 0; i < MSG_STACK_SIZE; i++ ){
    if( led->msg_stack[i] == msg ) {
      is_need_add_msg = false;
      break;
    }
    else {
      is_need_add_msg = true;
    }
  }
  if( is_need_add_msg ) {
    led->msg_stack[write_point] = msg;
    if ( write_point++ == MSG_STACK_SIZE ) {
      led->write_point = 0;
    }
    else {
      led->write_point = write_point;
    }
  }
}
void clear_msg_stack(Led *led){
  uint8_t i;
  for ( i = 0; i < MSG_STACK_SIZE; i++ ){
    led->msg_stack[i] = 0xff;
  }
}
bool is_stack_clear(Led *led){
  uint8_t i;
  for ( i = 0; i < MSG_STACK_SIZE; i++ ){
    if( led->msg_stack[i] != 0xff ){
      return false;
    }
  }
  return true;
}

void led_show_msg(Led *led){
  switch ( led->msg_state ) {
    case MSG_STATE_START:
    case MSG_STATE_READ_MSG:
      if( led->msg_stack[led->read_point] != 0xff ){
        led->cur_msg = led->msg_stack[led->read_point];
        led->cur_msg_bit = 0;
        led->msg_state = MSG_STATE_READ_BIT;
      }
      else {
        break;
      }
    case MSG_STATE_READ_BIT:
      led->time_counter = millis();
      led_on(led);
      if( led->cur_msg & (1 << led->cur_msg_bit) ) {
        led->msg_state = MSG_STATE_1;
      }
      else {
        led->msg_state = MSG_STATE_0;
      }
    case MSG_STATE_0:
      if( is_time_end(MSG_BIT0_DURATION, led->time_counter, millis()) ) {
        led->msg_state = MSG_STATE_END_BIT;
      }
      else {
        break;
      }
    case MSG_STATE_1:
      if( is_time_end(MSG_BIT1_DURATION, led->time_counter, millis())) {
        led->msg_state = MSG_STATE_END_BIT;
      }
      else {
        break;
      }
    case MSG_STATE_END_BIT:
      led_off(led);
      if( is_time_end(MSG_BIT_DURATION, led->time_counter, millis()) ) {
        led->cur_msg_bit++;
        if( led->cur_msg_bit < MSG_LEN ) {
          led->msg_state = MSG_STATE_READ_BIT;
        }
        else {
          led->msg_state = MSG_STATE_DELAY;
        }
      }
      else {
        break;
      }
    case MSG_STATE_DELAY:
      if( is_time_end(MSG_DELAY_DURATION, led->time_counter, millis()) ) {
        led->msg_state = MSG_STATE_READ_MSG;
        led->msg_stack[led->read_point] = 0xff;
        led->read_point ++;
        if( led->read_point == MSG_STACK_SIZE ) {
          led->read_point = 0;
        }
      }
      else {
        break;
      }
    default:
      break;
  }
}
void do_led(Led *led){
  switch(led->state){
    case LED_STATE_START:
    case LED_STATE_OFF:
      led_off(led);
      break;
    case LED_STATE_ON:
      led_on(led);
      break;
    case LED_STATE_SHOW_MSG:
      led_show_msg(led);
      break;
    default:
      break;
  }
}

/* button functions */
bool is_button_pressed(Button *btn){
  btn->btn_stack[btn->btn_point] = !DCB.digitalRead(btn->btn_pin);
  btn->btn_point ++;
  if(btn->btn_point == BTN_STACK_SIZE){
    btn->btn_point = 0;
  }
  uint8_t i;
  bool state = true;
  for ( i = 0; i < BTN_STACK_SIZE; i++ ){
    if( !btn->btn_stack[i] ) {
      state = false;
    }
  }
  return state;
}
