#include "dcb.h"
#include "fan.h"
#include "time.h"

#include "voltage.h"
#include "power.h"
#include "rtc.h"

#include "motherboard.h"
#include "nas.h"
#include "switch.h"
#include "controller.h"

#include "uart.h"

#include "ascii.h"

//#define DEBUG true
#define DEBUG false
#define print_dbg(s) if(DEBUG) { Serial.println(s); Serial.flush(); }

/* states */
bool is_voltage_norm = false;
bool is_battery_time_ok = false;
bool is_some_error = false;
bool is_halt = false;
bool halt_wait = false;
uint32_t halt_wait_time;

/* leds */
Led L1 = { L1_GRN_PIN, L1_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2_state = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2_battery = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L2_button = { L2_GRN_PIN, L2_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };
Led L3 = { L3_GRN_PIN, L3_RED_PIN, LED_STATE_START, MSG_STATE_START, millis() };

void led_blink(Led *led, uint8_t color, uint32_t blink_ms, uint32_t blink_wait_ms, bool one);
void setup_leds(){
  clear_msg_stack(&L1);
  clear_msg_stack(&L2_state);
  clear_msg_stack(&L2_battery);
  clear_msg_stack(&L2_button);
  clear_msg_stack(&L3);

  L1.blink_state = BLINK_STATE_START;
  L2_state.blink_state = BLINK_STATE_START;
  L2_battery.blink_state = BLINK_STATE_START;
  L2_button.blink_state = BLINK_STATE_START;
  L3.blink_state = BLINK_STATE_START;

  led_blink(&L2_state, GREEN_COLOR, 1, 0, true);
  led_blink(&L2_battery, GREEN_COLOR, 1, 0, true);
  led_blink(&L2_button, GREEN_COLOR, 1, 0, true);
}
void do_leds(){
  do_led(&L1);
  do_led(&L2_state);
  do_led(&L2_battery);
  do_led(&L2_button);
  do_led(&L3);
}
void show_current_state(uint8_t msg){
  if (DEBUG) {
    if( !is_some_error ){
      if( is_stack_clear(&L1) ) {
        add_msg(&L1, msg);
        L1.color = GREEN_COLOR;
        L1.state = LED_STATE_SHOW_MSG;
      }
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
  if( get_controller_status() != CONTROLLER_OFF ){
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
}
void led_on_with_color(Led *led, uint8_t color){
  led->color = color;
  led->state = LED_STATE_ON;
}
void led_off_no_color(Led *led){
  led->color - NO_COLOR;
  led->state = LED_STATE_OFF;
}
void led_blink(Led *led, uint8_t color, uint32_t blink_ms, uint32_t blink_wait_ms, bool one){
  if(one){
    led->state = LED_STATE_BLINK_ONE;
  }
  else{
    led->state = LED_STATE_BLINK;
  }
  led->color = color;
  led->blink_ms = blink_ms;
  led->blink_wait_ms = blink_wait_ms;
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
    print_dbg("check_i2c_err");
  }
}
bool check_i2c(){
  static uint8_t err_counter = 0;
  static bool i2c_hard_reset = false;
  static uint32_t i2c_reset_time_counter = 0;
  static uint32_t err_time_counter = 0;

  if( is_time_end(1000, err_time_counter, millis())){
    err_counter = 0;
  }
  if( i2c_hard_reset){
    if( is_time_end(I2C_RESET_DELAY, i2c_reset_time_counter, millis()) ){
      i2c_hard_reset = false;
      dacb_i2c_on();
    }
  }
  else if( is_dcb_i2c_err() ) {
    if ( (err_counter ++) > 3 ){
      err_counter = 0;
      i2c_hard_reset = true;
      dacb_i2c_off();
      i2c_reset_time_counter = millis();
    }
    else {
      err_time_counter = millis();
      Wire.begin();
      setup_dcb();
    }
  }
  /*
  else if( is_i2c_error(0x20) || is_i2c_error(0x68) ){
    Wire.begin();
    setup_dcb();
  }
  */
}

uint32_t critical_halt_time_counter;

#define LED1 72
#define LED2 73
void TC3_Handler(){
  static bool led = false;
  TC_GetStatus(TC1, 0);
  if(led){
    led = false;
  }
  else{
    led = true;
  }
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, led);
/*
  static uint8_t *stackptr = (uint8_t *)(*(volatile uint16_t *)(0x003d));
  static uint8_t *loop_addr = (uint8_t*)(( void(*)() ) &loop );
  static uint8_t *setup_addr = (uint8_t*)(( void(*)() ) &setup );
  static uint8_t *bat_addr = (uint8_t*)(( void(*)() ) &battery_off );
  static uint8_t *get_v_addr = (uint8_t*)(( void(*)() ) &get_voltage_res );
*/
  //Serial.print("loop_addr_size = ");
  //Serial.print(sizeof(loop_addr));
/*
  Serial.print(" setup_addr = ");
  Serial.print((uint32_t)setup_addr, HEX);

  Serial.print(" bat_addr = ");
  Serial.print((uint32_t)bat_addr, HEX);

  Serial.print(" loop_addr = ");
  Serial.print((uint32_t)loop_addr, HEX);


  Serial.print(" volt_addr = ");
  Serial.print((uint32_t)get_v_addr, HEX);
*/
// setup_addr = 807A5 bat_addr = 810DD loop_addr = 802A9 volt_addr = 8157D
/*
  Serial.print(" stack = ");
  //for(int i = 20; i > 0; i--){
  for(int i = 0; i < 20; i++){
    if(stackptr[i] < 0x10){
      Serial.print("0");
      Serial.print(stackptr[i], HEX);
    }
    else {
      Serial.print(stackptr[i], HEX);
    }
    Serial.print(" ");
  }

  Serial.println("");
  */
}

/* interrupt setup */
void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);

  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK / 128 / frequency;

  TC_SetRA(tc, channel, rc / 2);
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);

  tc->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
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

  led_on_with_color(&L3, RED_COLOR);
  show_current_state(0x00);

  uart_init();
  print_dbg("SETUP_END");
}

#define HALT_STATE_NO_HALT 0
#define HALT_STATE_HALT_NOW 1
#define HALT_STATE_WAIT 2
#define HALT_STATE_NO_WAIT 3
uint8_t halt_state = HALT_STATE_NO_HALT;
uint32_t halt_time_counter = millis();
uint32_t halt_show_time_counter = millis();
bool halt_show_color = true;



void loop(){

  if( is_start_voltage() ){
    if( halt_state != HALT_STATE_WAIT ){
      led_on_with_color(&L3, GREEN_COLOR);
    }
    is_voltage_norm = true;
  }
  else if( is_stop_voltage() ) {
    if( halt_state != HALT_STATE_WAIT ){
      led_on_with_color(&L3, RED_COLOR);
    }
    is_voltage_norm = false;
  }
  print_dbg("0");

  if( is_time_ok() ){
    is_battery_time_ok = true;
    L2_battery.state = LED_STATE_PAUSE;
  }
  else {
    is_battery_time_ok = false;
    led_blink(&L2_battery, RED_COLOR, 100, 3000, false);
  }
  print_dbg("1");

  switch ( get_controller_status() ){
    case CONTROLLER_OFF:
      power_off();
      battery_off();
      show_current_state(0x01);
      led_off_no_color(&L2_state);
      if( is_voltage_norm ){
        if( is_button_pressed(&S1) ){
          battery_on();
          power_on();
          controller_on();
          //led_blink(&L2_button, GREEN_COLOR, 100, 0, true);
        }
        if( is_button_pressed(&S2) ){
          led_blink(&L2_button, RED_COLOR, 1000, 0, true);
        }
      }
      else {
        if( is_button_pressed(&S1) || is_button_pressed(&S2) ){
          led_blink(&L2_button, RED_COLOR, 1000, 0, true);
        }
      }
      break;
    case CONTROLLER_ON:
      led_on_with_color(&L2_state, GREEN_COLOR);
      show_current_state(0x0f);
      if( is_button_pressed(&S2) ){
        controller_off();
        //led_blink(&L2_button, GREEN_COLOR, 100, 0, true);
      }
      else if( is_button_pressed(&S1) ){
        led_blink(&L2_button, RED_COLOR, 1000, 0, true);
      }
      break;
    case CONTROLLER_TURNS_ON:
      show_current_state(0x03);
      led_blink(&L2_state, GREEN_COLOR, 500, 500, false);
      if( is_button_pressed(&S1) ){
        //led_blink(&L2_button, GREEN_COLOR, 1000, 0, true);
      }
      else if( is_button_pressed(&S2) ){
        led_blink(&L2_button, RED_COLOR, 1000, 0, true);
      }
      break;
    case CONTROLLER_TURNS_OFF:
      show_current_state(0x07);
      led_blink(&L2_state, GREEN_COLOR, 1000, 1000, false);
      if( is_button_pressed(&S2) ){
        //led_blink(&L2_button, GREEN_COLOR, 1000, 0, true);
      }
      else if( is_button_pressed(&S1) ){
        led_blink(&L2_button, RED_COLOR, 1000, 0, true);
      }
      break;
    default:
      break;
  }
  print_dbg("2");

  switch(halt_state){
    case HALT_STATE_NO_HALT:
      if( !is_voltage_norm ){
        if( get_controller_status() != CONTROLLER_OFF ){
          controller_off();
          halt_state = HALT_STATE_HALT_NOW;
        }
      }
      break;
    case HALT_STATE_HALT_NOW:
      write_halt_time();
      if( is_battery_time_ok ){
        halt_time_counter = millis();
        halt_state = HALT_STATE_WAIT;
      }
      else {
        power_off();
        battery_off();
        halt_state = HALT_STATE_NO_HALT;
      }
      break;
    case HALT_STATE_WAIT:
      if( is_time_end(1000, halt_show_time_counter, millis()) ){
        halt_show_time_counter = millis();
        if( halt_show_color ) {
          halt_show_color = false;
          led_on_with_color(&L3, GREEN_COLOR);
        }
        else {
          halt_show_color = true;
          led_on_with_color(&L3, RED_COLOR);
        }
      }
      show_current_state(0x07);
      if( is_time_end(120000, halt_time_counter, millis()) ){
        power_off();
        battery_off();
        halt_state = HALT_STATE_NO_HALT;
      }
      break;
    default:
      break;
  }
  print_dbg("3");

  switch( get_uart_status() ) {
    case UART_INIT:
      uart_init();
      break;
    case UART_NAS:
      do_uart(&NAS_UART);
      break;
    case UART_SW:
      do_uart(&SW_UART);
      break;
    case UART_MB:
      do_uart(&MB_UART);
      break;
    default:
      break;
  }
  print_dbg("4");

  get_voltage_res();
  print_dbg("5");

  do_controller();
  print_dbg("6");

  do_leds();
  print_dbg("7");

  show_controller_error();
  print_dbg("8");

  check_i2c();
  print_dbg("9");


  if( is_button_pressed(&S2) ){
    if( is_time_end(10000, critical_halt_time_counter, millis())){
      controller_off();
      power_off();
      battery_off();
    }
  }
  else {
    critical_halt_time_counter = millis();
  }
  print_dbg("10");

/*
  static bool led = false;
  static uint32_t led_tmr = millis();
  if(is_time_end(500, led_tmr, millis())){
    led_tmr = millis();
    pinMode(LED2, OUTPUT);
    if(led){
      led = false;
      digitalWrite(LED2, led);
    }
    else{
      led = true;
      digitalWrite(LED2, led);
    }
  }
  */
}
