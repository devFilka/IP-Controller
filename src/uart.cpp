#include "uart.h"

void clear_uart(UARTClass *uart) {
  uart->write(ESC);   // ESC
  uart->print("[2J"); // clear screen
  uart->write(ESC);   // ESC
  uart->print("[H");  // cursor to home
}
void uart_usage() {
  clear_uart(&OUT_UART);
  OUT_UART.print(uart_usage_msg);
}

void uart_init() {
  OUT_UART.begin(OUT_UART_SPEED, OUT_UART_PARITY);
  uart_usage();
}
bool uart_connect( UARTClass *uart, uint32_t speed = 115200,  UARTClass::UARTModes config = SERIAL_8N1 ) {
  uart->begin(speed, config);
  clear_uart(&OUT_UART);
  OUT_UART.print("\rConnected\r");
}
void uart_disconnect(UARTClass *uart) {
  uart->end();
  clear_uart(&OUT_UART);
  OUT_UART.print("\rConnection closed\r");
}
void do_uart(UARTClass *uart) {
  static bool is_first_connected = true;
  if ( is_first_connected ) {
    is_first_connected = false;
    uart_connect(uart);
  }
  else {
    static char inc_byte = 0;
    static char rec_byte = 0;
    if( inc_byte != EOT ) {
      if( uart->available() > 0 ) {
        inc_byte = uart->read();
        OUT_UART.print(inc_byte);
      }
      if( OUT_UART.available() > 0 ) {
        rec_byte = OUT_UART.read();
        uart->print(rec_byte);
      }
    }
    else {
      is_first_connected = true;
      uart_disconnect(uart);
    }
  }
}

uint8_t get_cmd(String x) {
  if( x.length() < 2 ) {
    return CMD_NULL;
  }
  else {
    String cmd = x.substring(0, x.length() - 1);
    if (cmd == "HELP") {
      uart_usage();
      return CMD_HELP;
    }
    else if (cmd == "NAS") {
      return CMD_NAS;
    }
    else if (cmd == "MB") {
      return CMD_MB;
    }
    else if (cmd == "SW") {
      return CMD_SW;
    }
    else {
      OUT_UART.print("\r[ ERROR ]: unknown command - \"" + cmd + "\"\r");
      return CMD_ERR;
    }
  }
}
String read_uart_command() {
  static String out_command = "";
  static String return_command = "";
  static char inc_byte = 0;
  if (inc_byte != CR) {
    if (OUT_UART.available() > 0) {
      inc_byte = OUT_UART.read();
      out_command += inc_byte;
      OUT_UART.print(inc_byte);
    }
    return("");
  }
  else {
    inc_byte = 0;
    return_command = out_command;
    out_command = "";
    return(return_command);
  }
}
uint8_t get_uart_status() {
  static uint8_t uart_status = UART_START;

  switch (uart_status) {
    case UART_START:
    case UART_INIT:
      if( !OUT_UART ) {
        uart_init();
      }
      uart_status = UART_READ_CMD;
      break;
    case UART_READ_CMD:
      switch ( get_cmd( read_uart_command() ) ) {
        case CMD_MB:
          uart_status = UART_MB;
        case CMD_NAS:
          uart_status = UART_NAS;
        case CMD_SW:
          uart_status = UART_SW;
        default:
          break;
      }
      break;
    case UART_NAS:
      if( OUT_UART ){
        if( !NAS_UART ) {
          uart_status = UART_READ_CMD;
        }
      }
      break;
    case UART_SW:
      if( OUT_UART ){
        if( !SW_UART ) {
          uart_status = UART_READ_CMD;
        }
      }
      break;
    case UART_MB:
      if( OUT_UART ){
        if( !MB_UART ) {
          uart_status = UART_READ_CMD;
        }
      }
      break;
    default:
      uart_status = UART_HZ;
      break;
  }
  return uart_status;
}
