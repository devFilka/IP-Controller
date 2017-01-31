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
void power_off(){
  digitalWrite(POWER_ALL_PIN, LOW);
  pinMode( POWER_ALL_PIN, OUTPUT );
}
void power_on(){
  digitalWrite(POWER_ALL_PIN, HIGH);
  pinMode( POWER_ALL_PIN, OUTPUT );
}

/*  */
