#include "rtc.h"

TIME cur_time;
TIME halt_time;

bool is_rtc_state_ok;
/* RTC functions */
void read_cur_time(){
  RTC.read(cur_time.tm);
}

const char *monthName[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
bool getDate(const char *str){
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  cur_time.tm.Day = Day;
  cur_time.tm.Month = monthIndex + 1;
  cur_time.tm.Year = CalendarYrToTm(Year);
  return true;
}
bool getTime(const char *str){
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  cur_time.tm.Hour = Hour;
  cur_time.tm.Minute = Min;
  cur_time.tm.Second = Sec;
  return true;
}
bool is_rtc_first_setup_ok() {
  if (getDate(__DATE__) && getTime(__TIME__)) {
    if (RTC.write(cur_time.tm)) {
      return true;
    }
  }
  return false;
}
bool is_rtc_ok(){
  if (RTC.read(cur_time.tm)) {
    return true;
  } else if (RTC.chipPresent()) {
    if( is_rtc_first_setup_ok()){
      read_cur_time();
      return true;
    }
    else {
      return false;
    }
  } else {
    return false;
  }
}

void setup_time(){
  if ( is_rtc_ok() ){
    is_rtc_state_ok = true;
    flash_setup(cur_time);
    halt_time = read_halt_time(4);
  }
  else {
    is_rtc_state_ok = false;
  }
}

bool get_rtc_state(){
  return is_rtc_state_ok;
}

bool is_time_ok(){
  if( !is_rtc_state_ok ){
    return false;
  }

  read_cur_time();
  read_halt_time(4);

  if( cur_time.tm.Year - halt_time.tm.Year > 0 ){
    return true;
  }
  if( cur_time.tm.Month - halt_time.tm.Month > 0 ){
    return true;
  }

  if( cur_time.tm.Day - halt_time.tm.Day > 1 ){
    return true;
  }
  else if( cur_time.tm.Day - halt_time.tm.Day == 1 ){
    if( cur_time.tm.Hour > 3 ){
      return true;
    }
    else if( (24 - halt_time.tm.Hour + cur_time.tm.Hour) > 4 ){
      return true;
    }
    else if( (24 - halt_time.tm.Hour + cur_time.tm.Hour) == 4 ){
      if( cur_time.tm.Minute - halt_time.tm.Minute > 0 ){
        return true;
      }
    }
  }
  else{
    if( cur_time.tm.Hour - halt_time.tm.Hour > 4 ){
      return true;
    }
    if( cur_time.tm.Hour - halt_time.tm.Hour == 4 ){
      if( cur_time.tm.Minute - halt_time.tm.Minute >= 0 ){
        return true;
      }
    }
  }
  return false;
}

/* flash functions */
DueFlashStorage dueFlashStorage;

TIME read_halt_time(uint32_t address){
  byte* b = dueFlashStorage.readAddress(address);
  TIME halt;
  memcpy(&halt, b, sizeof(TIME));
  return halt;
}
void write_halt_time(TIME halt, uint32_t address) {
  byte b2[sizeof(TIME)]; // create byte array to store the struct
  memcpy(b2, &halt, sizeof(TIME)); // copy the struct to the byte array
  dueFlashStorage.write(address, b2, sizeof(TIME)); // write byte array to flash
}
void flash_setup(TIME cur_time){
  TIME halt;
  uint8_t codeRunningForTheFirstTime = dueFlashStorage.read(0);
  if (codeRunningForTheFirstTime) {
    halt = cur_time;
    halt.tm.Year --;
    write_halt_time(halt, 4);
    dueFlashStorage.write(0, 0);
  }
}
