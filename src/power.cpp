#include "power.h"

/* battery functions */
void battery_off(){
  digitalWrite(BATTERY_POWER_PIN, LOW);
  pinMode( BATTERY_POWER_PIN, OUTPUT );
}
void battery_on(){
  digitalWrite(BATTERY_POWER_PIN, HIGH);
  pinMode( BATTERY_POWER_PIN, OUTPUT );
}
/* power functions */
bool is_pow_on = false;
void power_off(){
  is_pow_on = false;
  digitalWrite(POWER_ALL_PIN, LOW);
  pinMode( POWER_ALL_PIN, OUTPUT );
}
void power_on(){
  is_pow_on = true;
  digitalWrite(POWER_ALL_PIN, HIGH);
  pinMode( POWER_ALL_PIN, OUTPUT );
}
bool is_power_all(){
  return is_pow_on;
}

/*  */
