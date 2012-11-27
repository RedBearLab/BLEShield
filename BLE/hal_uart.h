/*

Copyright (c) 2012 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef HAL_UART_H
#define HAL_UART_H
#include "hal_platform.h"

#define HAL_UART_PACKET_SIZE 3


void hal_uart_init();
void hal_uart_send_msg(char *p_msg);
void hal_uart_init_send(char *p_msg);
void hal_uart_send_data(uint8_t *p_msg_to_send, uint8_t size);
void hal_uart_init_send_data(uint8_t *p_msg_to_send, uint8_t size);
uint8_t hal_uart_read_byte();

#ifdef ENABLE_UART_INTERUPT_RCV
#define HAL_UART_MAX_LEN 5

/** Data type for received UART messages */
typedef struct hal_uart_data_t{
  uint8_t buffer_id;
  uint8_t msg_length;
  uint8_t buffer[HAL_UART_MAX_LEN];
} hal_uart_data_t;

void hal_uart_enable_rcv_interrupt();
void hal_uart_set_buffer(hal_uart_data_t *p_in_buffer);
void hal_uart_on_uart_isr();
extern void hal_uart_msg_rcv_hook(hal_uart_data_t *p_rcvd_msg);
#endif  //ENABLE_UART_INTERUPT_RCV
 
#endif //HAL_UART_H

