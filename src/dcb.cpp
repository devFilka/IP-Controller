#include "dcb.h"

PCF8574 DCB;

void setup_dcb() {
  DCB.begin(DACB_I2C_ADDRESS);

  DCB.pinMode(S1_PIN, INPUT_PULLUP);
  DCB.pinMode(S2_PIN, INPUT_PULLUP);

  DCB.pinMode(L1_GRN_PIN, OUTPUT);
  DCB.pinMode(L1_RED_PIN, OUTPUT);

  DCB.pinMode(L2_GRN_PIN, OUTPUT);
  DCB.pinMode(L2_RED_PIN, OUTPUT);

  DCB.pinMode(L3_GRN_PIN, OUTPUT);
  DCB.pinMode(L3_RED_PIN, OUTPUT);
}

typedef struct Led {
  const uint8_t num;
  const uint8_t green_pin;
  const uint8_t red_pin;
  uint8_t state;
  uint8_t color;
  uint8_t blink;
  uint8_t msg_stack[MSG_STACK_SIZE];
} ;
Led L1 = { 1, L1_GRN_PIN, L1_RED_PIN, LED_STATE_START };
Led L2 = { 2, L2_GRN_PIN, L2_RED_PIN, LED_STATE_START };
Led L3 = { 3, L3_GRN_PIN, L3_RED_PIN, LED_STATE_START };

void led_off(Led *led){
  DCB.pinMode(led->green_pin, OUTPUT);
  DCB.pinMode(led->red_pin, OUTPUT);
  DCB.digitalWrite(led->green_pin, LOW);
  DCB.digitalWrite(led->red_pin, LOW);
}
void led_on(Led *led){
  DCB.pinMode(led->green_pin, OUTPUT);
  DCB.pinMode(led->red_pin, OUTPUT);
  led->color & ( 1 << 0 ) ? DCB.digitalWrite(led->green_pin, LOW) : DCB.digitalWrite(led->green_pin, HIGH);
  led->color & ( 1 << 1 ) ? DCB.digitalWrite(led->red_pin, LOW)  : DCB.digitalWrite(led->red_pin, HIGH);
}
void led_blink(Led *led){
  static uint32_t time_counter[3];
  static uint8_t cur_msg[3];

}
void led_show_msg(){

}




/*
//uint32_t SOS = 0xa8db1500;
uint32_t SOS = 0xA8CCC2A0;


#define MAX_MILLIS 4294967295
#define t0  111
#define t1  444
#define tb  666
#define code_len 5

bool is_time_end(uint32_t t, uint32_t begin, uint32_t end) {
  if ( end >= begin ) {
    if ( (end - begin) >= t ) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    if ( (MAX_MILLIS - begin + end) >= t ) {
      return true;
    }
    else {
      return false;
    }
  }
}



uint8_t msg_buf[0x10];
uint8_t p_wr = 0, p_rd = 0;


void blink_msg(uint8_t msg) {
  msg_buf[p_wr] = msg;
  p_wr++;
  p_wr = p_wr & 0x0f;
}

void blink_next_blink() {
  static uint8_t cur_msg = 0;
  static uint32_t tmr_b, tmr = millis();
  static uint8_t i = 0;
  static uint8_t bl_st = 0;
  switch (bl_st) {
    case 0:
      if (p_wr != p_rd) {
        cur_msg = msg_buf[p_rd];
        p_rd++;
        p_rd = p_rd & 0x0f;
        i = 1;
        bl_st = 1;
        tmr = millis();
        digitalWrite(13, HIGH);
        if (cur_msg & 0x1) {
          tmr_b = t1;
        } else {
          tmr_b = t0;
        }
        cur_msg >>= 1;

      } else {
        break;
      }
    case 1:
      if (millis() - tmr > tmr_b) {
        digitalWrite(13, LOW);
        bl_st = 2;
      }
      break;
    case 2:
      if (millis() - tmr > tb) {
        //         digitalWrite(13, LOW);
        if (i < code_len) {
          tmr = millis();
          digitalWrite(13, HIGH);
          if (cur_msg & 0x1) {
            tmr_b = t1;
          } else {
            tmr_b = t0;
          }
          cur_msg >>= 1;
          bl_st = 1;
          i++;
        } else {
          bl_st = 0;
        }
      }
      break;
  }

}

void setup() {
  pinMode(13, OUTPUT);

  blink_msg(0x1a);
  blink_msg(0x1a);
  //  blink_msg(0x0);
  //  blink_msg(7);
}

void loop() {

  blink_next_blink();
}
*/
