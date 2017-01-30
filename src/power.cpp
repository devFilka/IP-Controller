#include "power.h"

/* battery functions */
void off_baterry(){
  digitalWrite(BATTERY_POWER_PIN, LOW);
}
void on_baterry(){
  digitalWrite(BATTERY_POWER_PIN, HIGH);
}

/*  */
