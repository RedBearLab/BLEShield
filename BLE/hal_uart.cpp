/*

Copyright (c) 2012 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "hal_platform.h"
#include "hal_uart.h"
#include "system.h"

void hal_uart_init() 
{
}


void hal_uart_send_msg(char *p_msg)
{
}

void hal_uart_init_send(char *p_msg)
{
}


void hal_uart_init_send_packet(uint8_t *p_msg_to_send)
{
}

void hal_uart_send_data(uint8_t *p_msg_to_send, uint8_t size)
{
}

void hal_uart_init_send_data(uint8_t *p_msg_to_send, uint8_t size)
{
}

uint8_t hal_uart_read_byte()
{
  return 0;
}

#ifdef ENABLE_UART_INTERUPT_RCV
static hal_uart_data_t *p_received_msg = NULL;
void hal_uart_enable_rcv_interrupt()
{
}

void hal_uart_disable_rcv_interrupt()
{
}

void hal_uart_set_buffer(hal_uart_data_t *p_in_buffer)
{
}

void read_msg_length(uint8_t rcv_byte)
{
}

void hal_uart_on_uart_isr()
{
}
#endif
