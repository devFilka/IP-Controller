#include "fan.h"

void set_fans( uint8_t percent ){
  uint8_t pwm = MIN_FAN_PWM - abs(MAX_FAN_PWM - MIN_FAN_PWM)*percent/100;
  pinMode(FAN_1_PIN, OUTPUT);
  pinMode(FAN_2_PIN, OUTPUT);
  analogWrite(FAN_1_PIN, pwm);
  analogWrite(FAN_2_PIN, pwm);
}
void fans_max(){
  pinMode(FAN_1_PIN, OUTPUT);
  pinMode(FAN_2_PIN, OUTPUT);
  digitalWrite(FAN_1_PIN, LOW);
  digitalWrite(FAN_2_PIN, LOW);
}
void fans_off(){
  pinMode(FAN_1_PIN, OUTPUT);
  pinMode(FAN_2_PIN, OUTPUT);
  digitalWrite(FAN_1_PIN, HIGH);
  digitalWrite(FAN_2_PIN, HIGH);
}
