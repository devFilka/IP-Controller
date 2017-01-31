#include "voltage.h"
#include "time.h"

uint32_t current_voltage;
uint32_t get_current_voltage_res(){
  return current_voltage;
}

void setup_spi() {
  pinMode(MISO_PIN,INPUT);
  pinMode(SCK_PIN,OUTPUT);
  pinMode(CONV_PIN,OUTPUT);
}
bool is_start_voltage(){
  static uint32_t time_counter = millis();
  if ( ( current_voltage < V_START_MAX ) && ( current_voltage > V_START_MIN ) ) {
    if( is_time_end(2000, time_counter, millis()) ){
      return true;
    }
    else {
      return false;
    }
  }
  time_counter = millis();
  return false;
}
bool is_stop_voltage(){
  if ( ( current_voltage > V_HALT_MAX ) || ( current_voltage < V_HALT_MIN ) ) {
    return true;
  }
  return false;
}
uint32_t fir(uint32_t res){
  static uint32_t fir_stack[FIR_STACK_SIZE];
  static uint8_t stack_point = 0;

  static uint32_t Utmp1 = 0;
  static uint32_t Utmp2 = 0;

  Utmp1 = fir_stack[stack_point];
  fir_stack[stack_point] = res;
  Utmp2 += fir_stack[stack_point];
	Utmp2 -= Utmp1;

  stack_point=(stack_point+1)%FIR_STACK_SIZE;

  return Utmp2 / FIR_STACK_SIZE;
}
void get_voltage_res(){
  static bool sck_status = false;
  static uint8_t freq_counter = 0;
  static uint8_t sck_counter = 0;
  static uint8_t spi_counter = 0;
  static uint8_t interrupt_counter = 0;
  static uint32_t spi_buffer = 0;
  static uint32_t ch1 = 0;
  static uint32_t ch2 = 0;
  static uint32_t SPI_RES = 0;
  static uint32_t time_counter = millis();

  if( is_time_end(SPI_FREQ, time_counter, millis())){
    time_counter = millis();
    if ( (freq_counter++) == 1){
      freq_counter = 0;
      sck_status = !sck_status;

      if (sck_status){
        if ( (sck_counter++) == 33) {
          sck_counter = 0;
        }
      }
      else {
        if( sck_counter < 33 ){
         spi_buffer = (spi_buffer << 1 ) | ( digitalRead(MISO_PIN) & 1 );
        }
      }
      digitalWrite(SCK_PIN, sck_status);
    }
    spi_counter++;
    if( spi_counter == 136) spi_counter = 0;
    if( spi_counter == 135) {
      ch1 = (spi_buffer >> 16 ) & (CH_MASK);
      ch2 = spi_buffer & (CH_MASK);
      SPI_RES =  ch2-ch1;
      if( SPI_RES > 16060) { SPI_RES = 16060; }
      if( SPI_RES < 1460)  { SPI_RES = 1460;  }
      current_voltage = fir( (16450 - SPI_RES) / 73 );
      spi_buffer = 0;
    }
    if( (spi_counter > 0) && (spi_counter < 3)) digitalWrite(CONV_PIN, HIGH);
    else digitalWrite(CONV_PIN, LOW);
  }
}
