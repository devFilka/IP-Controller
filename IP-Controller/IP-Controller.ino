//#include <DueFlashStorage.h>
//#include <flash_efc.h>
//#include <efc.h>

#include <Wire.h>    // Required for I2C communication
#include <PCF8574.h> // Required for PCF8574


#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>



/* OUTF8574 defines */
#define DACB_I2C_ADDRESS    0x20  // Define I2C Address for the PCF8574AT

#define S1_PIN     0
#define S2_PIN     1
#define L1_GRN_PIN 2
#define L1_RED_PIN 3
#define L2_GRN_PIN 4
#define L2_RED_PIN 5
#define L3_GRN_PIN 6
#define L3_RED_PIN 7

/* LED defines */
#define L1_LED 0
#define L2_LED 1
#define L3_LED 2

#define NO_COLOR     0 // 00
#define GREEN_COLOR  1 // 01
#define RED_COLOR    2 // 10
#define ORANGE_COLOR 3 // 11

/* UART defines */
#define MB_UART  Serial3
#define SW_UART  Serial2
#define NAS_UART Serial1
#define OUT_UART Serial

#define MB_UART_SPEED  115200
#define SW_UART_SPEED  115200
#define NAS_UART_SPEED 115200
#define OUT_UART_SPEED 115200

#define MB_UART_PARITY  SERIAL_8N1
#define SW_UART_PARITY  SERIAL_8N1
#define NAS_UART_PARITY SERIAL_8N1
#define OUT_UART_PARITY SERIAL_8N1

/* OUT UART commands defines*/
#define CMD_HELP     "HELP"
#define CMD_DEBUG    "DEBUG"

#define CMD_SEND_MB  "SEND_MB "
#define CMD_SEND_SW  "SEND_SW "
#define CMD_SEND_NAS "SEND_NAS "

/* MB defines */
#define MB_POWER_LED 23
#define MB_POWER_ON  24

/* NAS defines */
#define NAS_POWER_LED 71
#define NAS_POWER_ON  42

/* SW defines */
#define SW_POWER_LED 43
#define SW_POWER_ON  49

/* BATTERY defines */
#define BATTERY_STATUS 48
#define BATTERY_POWER  46

/* interrupt defines */
#define DCB_FREQ 100

/* functions header */
bool get_mb_status();
bool get_sw_status();
bool get_nas_status();

void get_buttons_status();
void power_on(uint8_t device);

void TC3_Handler();
void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency);

void led_on(uint8_t l, uint8_t color);
void show_leds();
void debug();
void uart_usage();

void print_message_from(USARTClass *p_Stream, String s);
void send_string_to(USARTClass *in_uart, String s, String msg);

void print_message_from_sw();
void print_message_from_mb();
void print_message_from_nas();

void send_string_to_sw(String s);
void send_string_to_mb(String s);
void send_string_to_nas(String s);
void check_power();

/* variables */

/* devices status */
bool MB_STATUS  = false;
bool SW_STATUS  = false;
bool NAS_STATUS = false;

/* debug */
bool DEBUG = true;

/* buttons */
uint8_t buttons[] = {S1_PIN, S2_PIN};
bool    btns_status[sizeof(buttons)] = {false};

/* leds */
uint8_t pow_leds[] = {MB_POWER_LED, NAS_POWER_LED, SW_POWER_LED};
bool    pow_status[sizeof(pow_leds)] = {false};

/* power */
uint8_t pow_on[] = {MB_POWER_ON, NAS_POWER_ON, SW_POWER_ON};
bool is_pow_on = false;

bool mb_pow_err = false;
bool sw_pow_err = false;
bool nas_pow_err = false;

/* UART functions */;
void setup_uarts();
void uart_usage();
void check_cmd(String x);
void read_uart_command();
void clear_uart(UARTClass *uart);
void do_uart(UARTClass *uart);


/* color */
/*                        L1        L2        L3     */
uint8_t led_data[] = { NO_COLOR, NO_COLOR, NO_COLOR };

uint8_t leds[3][2] = {
  { L1_GRN_PIN, L1_RED_PIN },
  { L2_GRN_PIN, L2_RED_PIN },
  { L3_GRN_PIN, L3_RED_PIN }
};


/* FAN */
#define FAN1 2
#define FAN2 13

#define MIN_FAN 255
#define MID_FAN 127
#define MAX_FAN 0

uint8_t fans[] = { FAN1, FAN2 };
uint8_t fans_val[sizeof(fans)];

void setup_fan() {
  pinMode(FAN1,OUTPUT);
  pinMode(FAN2,OUTPUT);
  set_fans(0);
}

void do_fans(){
  int i =0;
  for (i=0;i<sizeof(fans);i++){
    analogWrite(fans[i], fans_val[i]);
  }
}

void set_fans(uint8_t val){
  int i =0;
  for (i=0;i<sizeof(fans);i++){
    fans_val[i] = val;
  }
}
///////////////////////////////////////////






#define Ntap 31

#define DCgain 65536

uint16_t FIR_RES = 0;

uint16_t fir(int16_t NewSample) {
    int16_t FIRCoef[Ntap] = { 
            9,
           15,
           20,
           19,
            1,
          -47,
         -142,
         -287,
         -457,
         -557,
         -372,
          495,
         2680,
         7061,
        14645,
        19366,
        14645,
         7061,
         2680,
          495,
         -372,
         -557,
         -457,
         -287,
         -142,
          -47,
            1,
           19,
           20,
           15,
            9
    };

    static uint16_t x[Ntap]; //input samples
    uint32_t y=0;            //output sample
    int n;

    //shift the old samples
    for(n=Ntap-1; n>0; n--)
       x[n] = x[n-1];

    //Calculate the new output
    x[0] = NewSample;
    for(n=0; n<Ntap; n++)
        y += FIRCoef[n] * x[n];
    
    return y / DCgain;
}
























/* SPI */

#define MISO_PIN 74  //A25
#define SCK_PIN  76  //A27
#define CONV_PIN 47

#define SPI_FREQ 4000


#define V_MIN  75 // 5300+73*75 ~ 10700
#define V_MAX 150 // 5300

#define SPI_MIN 5300
#define SPI_MAX 10900

#define CH_MASK 0x3FFF

bool sck_status = false;
uint8_t sck_counter = 0;
uint8_t spi_counter = 0;
uint8_t interrupt_counter = 0;
uint8_t freq_counter = 0;

uint32_t spi_buffer = 0;

uint32_t ch1 = 0;
uint32_t ch2 = 0;

uint32_t SPI_RES = 0;

//TC1 ch 0
void TC3_Handler(){
  TC_GetStatus(TC1, 0);
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
    FIR_RES = fir(SPI_RES);
    spi_buffer = 0;
  }
  
  if( (spi_counter > 0) && (spi_counter < 3)) digitalWrite(CONV_PIN, HIGH);
  else digitalWrite(CONV_PIN, LOW);

}


void setup_spi() {
  pinMode(MISO_PIN,INPUT);
  pinMode(SCK_PIN,OUTPUT);
  pinMode(CONV_PIN,OUTPUT);
  
  startTimer(TC1, 0, TC3_IRQn, SPI_FREQ); //TC1 channel 0, the IRQ for that channel and the desired frequency
}

bool is_voltage_norm (uint32_t v) {
  if ( ( v < SPI_MAX ) && ( v > SPI_MIN ) ) {
    return true;
  }
  return false;
}



#define BATTERY_POWER  46


uint16_t mb_tries = 0;
uint16_t sw_tries = 0;
uint16_t nas_tries = 0;

uint16_t mb_errors = 0;
uint16_t sw_errors = 0;
uint16_t nas_errors = 0;

bool HALT = false;

/* interrupt functions */
/* DCB interrupt */
void TC4_Handler() {
  TC_GetStatus(TC1, 1);
  get_boards_status();
  get_buttons_status();
  check_power();
  show_leds();
  do_fans();

  if( !is_voltage_norm(FIR_RES) ){
    HALT = true;
  }
}

bool battery_status = false;

void setup_battery() {
  pinMode(BATTERY_POWER,OUTPUT);
  pinMode(BATTERY_STATUS,INPUT_PULLUP);
}

bool get_battery_status(){
  battery_status = digitalRead(BATTERY_STATUS);
  return battery_status;
}

void on_baterry(){
  digitalWrite(BATTERY_POWER, HIGH);
}

void off_baterry(){
  digitalWrite(BATTERY_POWER, LOW);
}
































uint8_t is_blink = 0;

bool ALL_DEV_STAT = false;
uint8_t die_counter = 0;

void check_power(){
  is_blink++;
  if(is_pow_on){
   // digitalWrite(SW_POWER_ON, LOW);

    if (!NAS_STATUS){
      if ( (nas_tries++) > 20 * 100 ) { nas_tries = 0; nas_errors++; }
      if ( nas_errors > 2 ) { nas_errors = 0; nas_pow_err = true; }
      if ( nas_tries < 50 ) { digitalWrite(NAS_POWER_ON, LOW); }
      if ( nas_tries > 50 ) { digitalWrite(NAS_POWER_ON, HIGH); }
    } else { 
      nas_tries = 19 * 100; 
      nas_pow_err = false; 
      digitalWrite(SW_POWER_ON, LOW);
    }

    //digitalWrite(SW_POWER_ON, LOW);

    if (!MB_STATUS){
      if ( (mb_tries++) > 5 * 100 ) { mb_tries = 0; mb_errors++; }
      if ( mb_errors > 2 ) { mb_errors = 0; mb_pow_err = true; }
      if ( mb_tries < 50 ) { digitalWrite(MB_POWER_ON, LOW); }
      if ( mb_tries > 50 ) { digitalWrite(MB_POWER_ON, HIGH); }
    } else { mb_tries = 4 * 100; mb_pow_err = false; }
    
    
    if (!MB_STATUS || /*!SW_STATUS ||*/ !NAS_STATUS){
      if (is_blink > 50) {led_data[1] = GREEN_COLOR;}
      if (is_blink > 100) { led_data[1] = NO_COLOR; is_blink = 0;}
    } else {led_data[1] = GREEN_COLOR; }
    if (mb_pow_err || nas_pow_err /*|| sw_pow_err*/){
      led_data[0] = RED_COLOR;
    } else { led_data[0] = NO_COLOR; }
  }
  
  else {
    digitalWrite(SW_POWER_ON, HIGH);
    if (MB_STATUS){
      if ( (mb_tries++) > 5 * 100 ) { mb_tries = 0; }
      if ( mb_tries < 50 ) { digitalWrite(MB_POWER_ON, LOW); }
      if ( mb_tries > 50 ) { digitalWrite(MB_POWER_ON, HIGH); }
    } else { mb_tries = 4 * 100; mb_pow_err = false; ALL_DEV_STAT = true;}
    
    if (NAS_STATUS){
      if ( (nas_tries++) > 20 * 100 ) { nas_tries = 0; }
      if ( nas_tries < 700 ) { digitalWrite(NAS_POWER_ON, LOW); }
      if ( nas_tries > 700 ) { digitalWrite(NAS_POWER_ON, HIGH); }
    } else { nas_tries = 19  * 100; nas_pow_err = false; ALL_DEV_STAT = true;}
    
    if (MB_STATUS || /*!SW_STATUS ||*/ NAS_STATUS){
      if (is_blink > 50) {led_data[1] = GREEN_COLOR;}
      if (is_blink > 100) { led_data[1] = NO_COLOR; is_blink = 0;}
    } else {led_data[1] = NO_COLOR; ALL_DEV_STAT = false; }
  }
  if( !ALL_DEV_STAT )  { die_counter ++;  }
  else { die_counter = 0; }
  
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




/* Display and Control Board = DCB */
PCF8574 DCB;

/* setup functions */
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


void setup_controller() {
  digitalWrite( MB_POWER_ON, LOW);
  //digitalWrite( MB_POWER_ON, HIGH);
  digitalWrite(NAS_POWER_ON, HIGH);
  digitalWrite( SW_POWER_ON, HIGH);
  
  pinMode( MB_POWER_ON, OUTPUT);
  pinMode(NAS_POWER_ON, OUTPUT);
  pinMode( SW_POWER_ON, OUTPUT);
  
  pinMode( MB_POWER_LED, INPUT_PULLUP);
  pinMode(NAS_POWER_LED, INPUT_PULLUP);
  pinMode( SW_POWER_LED, INPUT_PULLUP);


  

  
}

//////////////////////////////////////////////////////////////////////
//#include "uart.h"
//#include "ascii.h"


#define CR  13 /* Carriage return */
#define ESC 27 /* Escape */
#define EOT 4  /* End of transmission */

/* setup uarts */
void setup_uarts() {
  OUT_UART.begin(OUT_UART_SPEED, OUT_UART_PARITY);
  NAS_UART.begin(NAS_UART_SPEED, NAS_UART_PARITY);
  SW_UART.begin(SW_UART_SPEED, SW_UART_PARITY);
  MB_UART.begin(MB_UART_SPEED, MB_UART_PARITY);
  uart_usage();
}

void uart_usage() {
  OUT_UART.print("\rUSAGE :\r\
- HELP    - print this message \r\
- NAS     - Connect to NAS\r\
- SW      - Connect to SWitch\r\
- MB      - Connect to MotherBoard\r\
- Ctrl+D  - Close connection \r"\
);
}

void check_cmd(String x) {
  /* remove CR character */
  String cmd = x.substring(0, x.length() - 1);

  if (cmd == "HELP") {
    uart_usage();
  }
  else if (cmd == "NAS") {
    do_uart(&NAS_UART);
  }
  else if (cmd == "MB") {
    do_uart(&MB_UART);
  }
  else if (cmd == "SW") {
    do_uart(&SW_UART);
  }
/*
  else if (cmd == "FAN") {
    do_uart_fan();
  }
  */
  else {
    OUT_UART.print("\r[ ERROR ]: unknown command - \"" + cmd + "\"\r");
  }
}

void read_uart_command() {
  String out_command = "";
  char inc_byte = 0;
  while (inc_byte != CR) {
    
    if(HALT){return;}
    if (OUT_UART.available() > 0) {
      inc_byte = OUT_UART.read();
      out_command += inc_byte;
      OUT_UART.print(inc_byte);
    }
  }
  check_cmd(out_command);
}

void clear_uart(UARTClass *uart){
  uart->write(ESC);   // ESC 
  uart->print("[2J"); // clear screen 
  uart->write(ESC);   // ESC 
  uart->print("[H");  // cursor to home 
}

void do_uart(UARTClass *uart) {
  OUT_UART.print("\rConnected\r");
  char inc_byte = 0;
  while (inc_byte != EOT) {
    if(HALT){return;}
    if (uart->available() > 0) {
      inc_byte = uart->read();
      OUT_UART.print(inc_byte);
    }
    if (OUT_UART.available() > 0) {
      inc_byte = OUT_UART.read();
      uart->print(inc_byte);
    }
  }  
  clear_uart(&OUT_UART);
  OUT_UART.print("\rConnection closed\r");
}
//////////////////////////////////////////////////////////////////////

bool get_mb_status(){
  return !digitalRead( MB_POWER_LED  );
}
bool get_sw_status(){
  return !digitalRead( SW_POWER_LED  );
}
bool get_nas_status(){
  return !digitalRead( NAS_POWER_LED  );
}

void get_boards_status() {
  MB_STATUS  = get_mb_status();
  SW_STATUS  = get_sw_status();
  NAS_STATUS = get_nas_status();
}

void get_buttons_status() {
  int i = 0;
  for (i = 0; i < sizeof(buttons); i++)
    btns_status[i] = !DCB.digitalRead(buttons[i]) ; // inverted
    
  if(btns_status[0]) { 
    is_pow_on = true;
    mb_tries = 40;
    nas_tries = 190;
    mb_errors = 0;
    nas_errors = 0;
    mb_pow_err = 0;
    sw_pow_err = 0;
    nas_pow_err = 0;
    led_data[0] = NO_COLOR;
  }
  else if(btns_status[1]) { 
    is_pow_on = false;
    mb_tries = 0;
    nas_tries = 0;
    mb_errors = 0;
    nas_errors = 0;
    mb_pow_err = 0;
    sw_pow_err = 0;
    nas_pow_err = 0;
    led_data[0] = NO_COLOR;
  }
}


void led_on(uint8_t l, uint8_t color) {
  int i = 0;
  for ( i = 0; i < 2; i++)
    color & ( 1 << i ) ? DCB.digitalWrite(leds[l][i], LOW) : DCB.digitalWrite(leds[l][i], HIGH); // inverted
}

void show_leds() {
  int i = 0;
  for ( i = 0; i < 3; i++ )
    led_on(i, led_data[i]);
}

void setup() {
  //on_baterry();
  
  off_baterry();
  setup_battery();
  
  setup_fan();
  setup_uarts();
  setup_dcb();
  setup_spi();


  led_data[2] = RED_COLOR;
  show_leds();
  
  delay(10000);
  
  while( !is_voltage_norm(FIR_RES) ){
    led_data[2] = RED_COLOR;
    show_leds();
  }
  
  led_data[2] = GREEN_COLOR;
  show_leds();
  startTimer(TC1, 1, TC4_IRQn, DCB_FREQ);
  set_fans(MAX_FAN);


}

// RTC begin
tmElements_t tm;
tmElements_t last_halt;





const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void rtc_setup() {
  bool parse=false;
  bool config=false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }
}

bool getTime(const char *str){
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str){
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}









/*
#define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year 
#define  CalendarYrToTm(Y)   ((Y) - 1970)




if (RTC.read(tm)) {
  print2digits(tm.Hour);
  print2digits(tm.Minute);
  print2digits(tm.Second);
  Serial.print(tm.Day);
  Serial.print(tm.Month);
  Serial.print(tm.Year);
} else {
  if (RTC.chipPresent()) {
    Serial.println("The DS1307 is stopped.  Please run the SetTime");
    Serial.println("example to initialize the time and begin running.");
    Serial.println();
  } else {
    Serial.println("DS1307 read error!  Please check the circuitry.");
    Serial.println();
  }
*/























byte *writeAddr = (byte *)(IFLASH1_ADDR + IFLASH1_SIZE - sizeof(tmElements_t));

#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;

// The struct of the configuration.
struct Configuration {
  uint8_t a;
  uint8_t b;
  int32_t bigInteger;
  char* message;
  char c;
};

// initialize one struct
Configuration configuration;

void write_() {
  byte b2[sizeof(tmElements_t)];
  memcpy(b2, &tm, sizeof(tmElements_t));
  dueFlashStorage.write(*writeAddr, b2, sizeof(tmElements_t)); // write byte array to flash
}




void aloop() {
  /* read configuration struct from flash */
  byte* b = dueFlashStorage.readAddress(*writeAddr); // byte array which is read from flash at adress 4

  memcpy(&last_halt, b, sizeof(Configuration)); // copy byte array to temporary struct

  // print the content
/*  Serial.print("a:");
  Serial.print(configurationFromFlash.a);

  Serial.print(" b:");
  Serial.print(configurationFromFlash.b);

  Serial.print(" bigInteger:");
  Serial.print(configurationFromFlash.bigInteger);

  Serial.print(" message:");
  Serial.print(configurationFromFlash.message);

  Serial.print(" c:");
  Serial.print(configurationFromFlash.c);
  Serial.println();
  Serial.println();*/

  /* change some values in the struct and write them back */
  // increment b by 1 (modulus 100 to start over at 0 when 100 is reached)
  //!! configurationFromFlash.b = (configurationFromFlash.b+1) % 100;

  // change the message
  //!! String message = configurationFromFlash.message;
  //!! if (configurationFromFlash.message == "Hello world!")
  //!!   configurationFromFlash.message = "Hello Arduino Due!";
  //!! else
  //!!  configurationFromFlash.message = "Hello world!";

  // write configuration struct to flash at adress 4
  //!! byte b2[sizeof(Configuration)]; // create byte array to store the struct
  //!! memcpy(b2, &configurationFromFlash, sizeof(Configuration)); // copy the struct to the byte array
  //!! dueFlashStorage.write(4, b2, sizeof(Configuration)); // write byte array to flash


}






































void read_halt_time(){
  
}

void write_halt_time(){

}


bool is_time_ok(){
  
  RTC.read(tm);
  
  
  return true;  
}



void loop() {
  HALT = false;

  while( !is_voltage_norm(FIR_RES) ){
    led_data[2] = RED_COLOR;
    show_leds();
  }
  setup_controller();

  while ( !is_time_ok() ){
    
  }

  led_data[2] = GREEN_COLOR;
  on_baterry();
  NVIC_EnableIRQ(TC4_IRQn);

  
  read_uart_command();

  delay(5000);

  if (!is_voltage_norm(FIR_RES)) {
    write_halt_time();
    
    led_data[0] = NO_COLOR;
    led_data[2] = ORANGE_COLOR;
    show_leds();
  
    NVIC_DisableIRQ(TC3_IRQn);
    NVIC_DisableIRQ(TC4_IRQn);
    
    is_pow_on = false;
    mb_tries = 0;
    nas_tries = 0;
    mb_errors = 0;
    nas_errors = 0;
    mb_pow_err = 0;
    sw_pow_err = 0;
    nas_pow_err = 0;
      
    //NVIC_EnableIRQ(TC4_IRQn);
    
    delay(120000);
    
    off_baterry();
    
    delay(5000);
  }
  NVIC_EnableIRQ(TC3_IRQn);
}





