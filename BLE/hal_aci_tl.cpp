/*

Copyright (c) 2012 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "hal_platform.h"
#include "hal_aci_tl.h"
#include "system.h"
//#include "assertions.h"
#include "lib_mem.h"

#define HAL_IO_RADIO_REQN 9
#define HAL_IO_RADIO_RDY 8

static hal_aci_data_t received_data;
static volatile hal_aci_data_t data_to_send;
static volatile bool spi_transmit_requested;
static uint8_t spi_readwrite(uint8_t aci_byte); 

void hal_aci_tl_io_config()
{
  pinMode(HAL_IO_RADIO_REQN, OUTPUT);  
  digitalWrite(HAL_IO_RADIO_REQN, HIGH);

  pinMode(HAL_IO_RADIO_RDY, INPUT);  
}

void hal_aci_tl_init()
{
  received_data.buffer[0] = 0;

  digitalWrite(HAL_IO_RADIO_REQN, HIGH);

  spi_transmit_requested = false;
  data_to_send.buffer[0] = 0;
}

bool hal_aci_tl_send(hal_aci_data_t *p_aci_cmd)
{
  uint8_t length = p_aci_cmd->buffer[0];
  if (!spi_transmit_requested)
  {
//    ASSERT(ERROR_CODE_HAL_ACI_TL_OVERFLOW,(p_aci_cmd->buffer[0] <= HAL_ACI_MAX_LENGTH));
    if (length > HAL_ACI_MAX_LENGTH)
    {
      return(false);
    }
    {
      bool  is_interrupt_enabled_before_send = ARE_INTERRUPTS_ENABLED();
      DISABLE_INTERRUPTS();                           /*disable interrupts to protects the modification of the buffer pointer*/
      lib_mem_copy((uint8_t *) data_to_send.buffer, p_aci_cmd->buffer, length+1);
      spi_transmit_requested = true; // Request transmission
      if (is_interrupt_enabled_before_send)
      {
        ENABLE_INTERRUPTS();                         /*eventually re-enable the interrupts if they were enabled*/
      }
    }

    digitalWrite(HAL_IO_RADIO_REQN, LOW);

    return(true);
  }
  else
  {
    return(false);
  }
}

void hal_aci_tl_poll_rdy_line(void)
{
  uint8_t byte_cnt;
  uint8_t byte_sent_cnt;
  uint8_t max_bytes;
  bool is_transmit_finished = false;
  
  if ( 1 == digitalRead(HAL_IO_RADIO_RDY) )
  {
    return;
  }
  

  digitalWrite(HAL_IO_RADIO_REQN, LOW);

  
  // Send length, receive header
  byte_sent_cnt = 0;
  received_data.status_byte = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  // Send first byte, receive length from slave
  received_data.buffer[0] = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  if (0 == data_to_send.buffer[0])
  {
    max_bytes = received_data.buffer[0];
  }
  else
  {
    // Set the maximum to the biggest size. One command byte is already sent
    max_bytes = (received_data.buffer[0] > (data_to_send.buffer[0] - 1)) 
      ? received_data.buffer[0] : (data_to_send.buffer[0] - 1);
  }

  if (max_bytes > HAL_ACI_MAX_LENGTH)
  {
    max_bytes = HAL_ACI_MAX_LENGTH;
  }

  // Transmit/receive the rest of the packet 
  for (byte_cnt = 0; byte_cnt < max_bytes; byte_cnt++)
  {
    received_data.buffer[byte_cnt+1] =  spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  }
  /* Release REQN */
  digitalWrite(HAL_IO_RADIO_REQN, HIGH);

//  ASSERT(ERROR_CODE_HAL_ACI_TL_STATUS_BYTE,(0 != received_data.status_byte));
  if (spi_transmit_requested)
  {
    data_to_send.buffer[0] = 0;
    spi_transmit_requested = false;
  }
  
  /* valid Rx available or transmit finished*/
  hal_aci_tl_msg_rcv_hook(&received_data);
}

static uint8_t spi_readwrite(const uint8_t aci_byte)
{
  SPDR = aci_byte;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
  {
  };
  return SPDR;                    // return the received byte  
}

