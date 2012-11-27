/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision$
 */                                              
/* Attention! 
*  To maintain compliance with Nordic Semiconductor ASA�s Bluetooth profile 
*  qualification listings, this section of source code must not be modified.
*/
#include "hal_platform.h"
#include "lib_traces.h"
#include "hal_uart.h"

#if ((DEBUG_ENABLE!=NO_TRACES)||(ASSERT_ENABLE!=NO_TRACES))


#if ((DEBUG_ENABLE==CODED_TRACES)||(ASSERT_ENABLE==CODED_TRACES))
uint8_t log_packet[3];
#endif

void lib_traces_log_send(uint8_t *p_data_to_send, uint8_t size)
{
  hal_uart_init_send_data(p_data_to_send, size);
}

void lib_traces_log_send_text(char *p_text_to_send)
{
  hal_uart_init_send(p_text_to_send);
}

void lib_traces_log_send_code(uint8_t *p_data_to_send)
{
  hal_uart_init_send_data(p_data_to_send, NB_BYTES_CODED_TRACES);
}

void lib_traces_clear()
{
  uint8_t clear_code = 0x27;
  hal_uart_init_send_data(&clear_code, 1);
}

#endif // ((DEBUG_ENABLE!=NO_TRACES)||(ASSERT_ENABLE!=NO_TRACES))

#if ((ASSERT_ENABLE==CODED_TRACES)||(DEBUG_ENABLE==CODED_TRACES)||(TEST_ENABLE==CODED_TRACES))

void lib_traces_init()
{
  log_packet[0] = 0xAA;
  log_packet[1] = 0xAA;
  log_packet[2] = 0xAA;
  lib_traces_log_send_code(log_packet);
}

void lib_traces_wait_for_start()
{
  uint8_t read_byte=0x00;
  hal_uart_init();
  do{
    read_byte = hal_uart_read_byte();
  }while(LIB_TRACE_START_CMD != read_byte);
}

#endif //((ASSERT_ENABLE==CODED_TRACES)||(DEBUG_ENABLE==CODED_TRACES)||(TEST_ENABLE==CODED_TRACES))
/** @} */
