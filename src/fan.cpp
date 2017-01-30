#include "fan.h"

void set_fans( uint8_t percent ){
  uint8_t pwm = MIN_FAN_PWM - abs(MAX_FAN_PWM - MIN_FAN_PWM)*percent/100;
  analogWrite(FAN_1_PIN, pwm);
  analogWrite(FAN_2_PIN, pwm);
}
void fans_max(){
  digitalWrite(FAN_1_PIN, LOW);
  digitalWrite(FAN_2_PIN, LOW);
}
void fans_off(){
  digitalWrite(FAN_1_PIN, HIGH);
  digitalWrite(FAN_2_PIN, HIGH);
}
