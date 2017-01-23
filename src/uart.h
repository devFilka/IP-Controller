#ifndef UART_H
#define UART_H

/* Arduino header */
#include <Arduino.h>

/* UART defines */
#define OUT_UART Serial
#define NAS_UART Serial1
#define SW_UART  Serial2
#define MB_UART  Serial3

#define OUT_UART_SPEED 115200
#define NAS_UART_SPEED 115200
#define SW_UART_SPEED  115200
#define MB_UART_SPEED  115200

#define OUT_UART_PARITY SERIAL_8N1
#define NAS_UART_PARITY SERIAL_8N1
#define SW_UART_PARITY  SERIAL_8N1
#define MB_UART_PARITY  SERIAL_8N1

/* UART constants */
const String uart_usage_msg = "\rUSAGE :\r\
- HELP    - print this message \r\
- NAS     - Connect to NAS\r\
- SW      - Connect to SWitch\r\
- MB      - Connect to MotherBoard\r\
- Ctrl+D  - Close connection \r";

/* USRT states */
#define UART_START 0
#define UART_INIT 1
#define UART_READ_CMD 2
#define UART_NAS 3
#define UART_SW 4
#define UART_MB 5
#define UART_HZ 100

/* UART commands */
#define CMD_HELP 0
#define CMD_MB 1
#define CMD_NAS 2
#define CMD_SW 3
#define CMD_ERR 100
#define CMD_NULL 200

/* UART functions */;
void clear_uart(UARTClass *uart);
bool uart_init();
void uart_usage();
void do_uart(UARTClass *uart);
uint8_t get_cmd(String x);
String read_uart_command();
uint8_t get_uart_status();

#endif
