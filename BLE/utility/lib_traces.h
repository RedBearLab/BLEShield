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
*  To maintain compliance with Nordic Semiconductor ASA’s Bluetooth profile 
*  qualification listings, this section of source code must not be modified.
*/ 
/** @file
 *
 */

/** @defgroup lib_traces lib_traces
* @brief define traces information
* @{
* @ingroup lib
* @brief Traces library
* @details 
* This module implements the function to send traces/log information over the UART. A trace can be of type INFO or ERROR. 
* 
* The interface for this module is made of macros which have different implementation depending on the value of DEBUG_ENABLE
* and ASSERT_ENABLE compile flags. Those flags can be set to NO_TRACES and then no code is associated to the macros, 
* or CODED_TRACES to enable the sending of coded 3 bytes traces.
* 
* <h4>Requirements</h4>
* <h3>HAL</h3>
* This library uses @ref hal_uart functions for sending traces over the UART:
*  - @ref hal_uart_init_send_data() which initializes the UART and sends the given number of bytes from the given pointer on the UART.
*  - @ref hal_uart_init_send() which initialize the UART and sends the given NULL terminated string over the UART.
*  - @ref hal_uart_read_byte() which reads one byte from the UART (blocking read until one byte is received).
*/

#ifndef LIB_TRACES_H
#define LIB_TRACES_H
#include "hal_platform.h"

/*Possible traces level*/
#define CODED_TRACES                    1
#define NO_TRACES                       0

#ifndef TEST_ENABLE
#define TEST_ENABLE NO_TRACES
#endif //TEST_ENABLE

#ifndef DEBUG_ENABLE
#define DEBUG_ENABLE NO_TRACES
#endif //DEBUG_ENABLE
#ifndef ASSERT_ENABLE
#define ASSERT_ENABLE NO_TRACES
#endif //ASSERT_ENABLE


#define NB_BYTES_CODED_TRACES 3

#if (ASSERT_ENABLE==CODED_TRACES)
#include "error_codes.h"
#endif //(ASSERT_ENABLE==CODED_TRACES)

#if (DEBUG_ENABLE==CODED_TRACES)
#include "info_codes.h"
#endif //(DEBUG_ENABLE==CODED_TRACES)

/* traces Codes format : first byte : bits 7-6 : Type of message ('00' TEST, '01' ASSERT and '11' DEBUG). Rest is for the message.*/
#define LIB_TRACES_MASK_TYPE_CODE   0xC0
#define LIB_TRACES_MASK_CODE        0x3F

#define LIB_TRACES_TYPE_TEST        0x00
#define LIB_TRACES_TYPE_INFO        0xC0
#define LIB_TRACES_TYPE_ERROR       0x40

#define LIB_TRACE_START_CMD        0xA6


/**@brief Init trace system
 *  @details
 *  This macros sends a specific set of data to allow synchronisation. Call it at reset of your application.
*/
#if ((DEBUG_ENABLE==CODED_TRACES)||(ASSERT_ENABLE==CODED_TRACES))
void lib_traces_init(void);
#define LIB_TRACES_INIT() do{\
  lib_traces_init();\
} while(0)
#else
#define LIB_TRACES_INIT()                             do { } while(0)
#endif

#if (ASSERT_ENABLE==NO_TRACES)
/**@brief Logs an error
 *  @details
 *  This macro logs an error trace.
 *  @param log_code Error code sent if ASSERT_ENABLE compile flag is set to CODED_TRACES.
*/
#define LIB_TRACES_LOG_ERROR(log_code)                do { } while(0)
/**@brief Logs an error with one variable
 *  @details
 *  This macro logs an error trace containing one variable.
 *  @param log_code Error code sent if ASSERT_ENABLE compile flag is set to CODED_TRACES.
 *  @param var Variable sent (as third byte if in CODED_TRACES).
*/
#define LIB_TRACES_LOG_ERROR_1VAR(log_code, var)      do { } while(0)
#endif //(ASSERT_ENABLE==NO_TRACES)

#if (DEBUG_ENABLE==NO_TRACES)
/**@brief Logs information
 *  @details
 *  This macros logs an information trace.
 *  @param log_code Information code sent if DEBUG_ENABLE compile flag is set to CODED_TRACES.
*/
#define LIB_TRACES_LOG_INFO(log_code)                 do { } while(0)
/**@brief Logs information with one variable
 *  @details
 *  This macros logs an information trace containing one variable.
 *  @param log_code Information code sent if ASSERT_ENABLE compile flag is set to CODED_TRACES.
 *  @param var Variable sent (as third byte if in CODED_TRACES).
*/
#define LIB_TRACES_LOG_INFO_1VAR(log_code, var)       do { } while(0)


#endif //(DEBUG_ENABLE==NO_TRACES)


#if ((ASSERT_ENABLE==CODED_TRACES)||(DEBUG_ENABLE==CODED_TRACES))
extern void lib_traces_clear(void);
extern void lib_traces_log_send(uint8_t *p_data_to_send, uint8_t size);
extern void lib_traces_log_send_code(uint8_t *p_data_to_send);
extern void lib_traces_log_send_text(char *p_text_to_send);

extern uint8_t log_packet[NB_BYTES_CODED_TRACES];
#endif //((ASSERT_ENABLE==CODED_TRACES)||(DEBUG_ENABLE==CODED_TRACES))

#if ((ASSERT_ENABLE==NO_TRACES)&&(DEBUG_ENABLE==NO_TRACES)&&(TEST_ENABLE==NO_TRACES))
/**@brief Waits for a start command.
 *  @details
 *  This macros waits until a start command (0xA6) is received on the UART.
*/
#define LIB_TRACES_WAIT_FOR_START()         do { } while(0)
#else
extern void lib_traces_wait_for_start(void);
#define LIB_TRACES_WAIT_FOR_START()do{\
  lib_traces_wait_for_start();\
} while(0)

#endif //((ASSERT_ENABLE==NO_TRACES)&&(DEBUG_ENABLE==NO_TRACES)&&(TEST_ENABLE==NO_TRACES))


#if ASSERT_ENABLE==CODED_TRACES
/**@brief Logs an error
 *  @details
 *  This macros logs an error trace.
 *  @param log_code error code sent if ASSERT_ENABLE compile flag is set to CODED_TRACES.
*/
#define LIB_TRACES_LOG_ERROR(log_code)do{                    \
  log_packet[0] = LIB_TRACES_TYPE_ERROR;\
  log_packet[1] = (log_code);\
  log_packet[2] = 0x00;\
  lib_traces_log_send_code(log_packet);\
} while(0)
/**@brief Logs an error with one variable
 *  @details
 *  This macros logs an error trace containing one variable.
 *  @param log_code error code sent if ASSERT_ENABLE compile flag is set to CODED_TRACES.
 *  @param var variable sent (as third byte if in CODED_TRACES).
*/
#define LIB_TRACES_LOG_ERROR_1VAR(log_code, var)do{                    \
  log_packet[0] = LIB_TRACES_TYPE_ERROR;\
  log_packet[1] = (log_code);\
  log_packet[2] = (var);\
  lib_traces_log_send_code(log_packet);\
} while(0)

#endif //ASSERT_ENABLE==CODED_TRACES

#if DEBUG_ENABLE==CODED_TRACES

/**@brief Logs information
 *  @details
 *  This macros logs an information trace.
 *  @param log_code information code sent if DEBUG_ENABLE compile is flag set to CODED_TRACES.
*/
#define LIB_TRACES_LOG_INFO(log_code)do{                    \
  log_packet[0] = LIB_TRACES_TYPE_INFO;\
  log_packet[1] = (log_code);\
  log_packet[2] = 0x00;\
  lib_traces_log_send_code(log_packet);\
} while(0)

/**@brief Logs information with one variable
 *  @details
 *  This macros logs an information trace containing one variable.
 *  @param log_code information code sent if ASSERT_ENABLE compile is flag set to CODED_TRACES.
 *  @param var variable sent (as third byte if in CODED_TRACES).
*/
#define LIB_TRACES_LOG_INFO_1VAR(log_code, var)do{                    \
  log_packet[0] = LIB_TRACES_TYPE_INFO;\
  log_packet[1] = (log_code);\
  log_packet[2] = (var);\
  lib_traces_log_send_code(log_packet);\
} while(0)
#endif //DEBUG_ENABLE==CODED_TRACES

#endif //LIB_TRACES_H
/** @} */
