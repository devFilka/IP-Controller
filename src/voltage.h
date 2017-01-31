#ifndef VOLTAGE_H
#define VOLTAGE_H

/* Arduino header */
#include "Arduino.h"

/* SPI defines */
#define MISO_PIN 74  //A25
#define SCK_PIN  76  //A27
#define CONV_PIN 47

/* SPI constants */
#define SPI_FREQ 1
#define CH_MASK 0x3FFF
#define FIR_STACK_SIZE 10

/* Voltage constants */
#define V_START_MIN 75
#define V_START_MAX 150
#define V_HALT_MIN 70
#define V_HALT_MAX 155

/* Voltage functions */
uint32_t get_current_voltage_res();
uint32_t fir(uint32_t res);
bool is_start_voltage();
bool is_stop_voltage();
void get_voltage_res();
void setup_spi();

#endif
