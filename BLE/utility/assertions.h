/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.leas
 *
 * $LastChangedRevision$
 */       
/* Attention! 
*  To maintain compliance with Nordic Semiconductor ASA’s Bluetooth profile 
*  qualification listings, this section of source code must not be modified.
*/ 
/** @file
 */

/** @defgroup lib_assert lib_assert
* @brief An assertions library
* @{
* @ingroup lib
* @brief Assertion library
* @details 
* This module implements macros for assertions and uses hook functions that have to be implemented by the client's application
* upon assertions conditions.
*/

#ifndef ASSERT_H
#define ASSERT_H
#include "hal_platform.h"
#include "assertions_setup.h"
#include "lib_traces.h"

#ifndef ASSERT_ENABLE
#define ASSERT_ENABLE NO_TRACES
#endif //ASSERT_ENABLE


#if ASSERT_ENABLE!=NO_TRACES

/** @brief Hook assertion function.
 *  @details This function is called whenever an assertion is triggered.
 *  The implementation of this function must be customized by the end user.
 *  @param line Integer holding the line number on which the assertion was called.
 *  @param *file String holding the name of the file in which the assertion was called.
 *  @param assert_code Error code to use for this assert.
 */
void assert_handler(uint8_t assert_code, uint16_t line, char* file);

/** @brief Hook function to memorize an assertion.
 *  @details This function is called when an assertion is triggered from the ASSERT_MSG_IT() macro (to be used from interrupt
 *  in order to avoid taking time during the interruption).
 *  The implementation of this function must be customized by the end user.
 *  @param line Integer holding the line number on which the assertion was called.
 *  @param *file String holding the name of the file in which the assertion was called.
 *  @param assert_code Error code to use for this assert.
 */
void set_assert_msg(uint8_t assert_code, uint16_t line, char* file);

/** @brief Hook function to check if an assertion has occurred.
 *  @details This function is called to check if any assertions have occurred during interruption (by a call
 *  to the ASSERT_MSG_IT() macro).
 *  The implementation of this function must be customized by the end user.
 */
void check_assert(void);

/** @brief Check assert macro.
 *  @details This macros check if a call to ASSERT_MSG_IT has been made (calls check_assert() function).
 */
#define CHECK_ASSERT()\
  check_assert()

/** @brief Macro for calling an assert.
 *  @details This macro can be called by any firmware module in order to trigger an assertion.
 *  @param assert_code error code to use for this assert.
 *  @param condition test that if false will trigger a call to the assert_handler() function.
 */
#define ASSERT(assert_code, condition) \
  do{                                                         \
    if(!(condition))                                          \
    {                                                         \
      assert_handler(assert_code, ASSERT_LINE, ASSERT_FILE);  \
    }                                                         \
  } while(0)

/** @brief Macro for calling an assert from an interrupt.
 *  @details This macro can be called from an interrupt routine to set an assert message/code if the condition is false.
 *  @param assert_code error code to use for this assert.
 *  @param condition test that if false will trigger a call to the assert_handler() function.
 */
#define ASSERT_MSG_IT(assert_code, condition)\
  do{                                                         \
    if(!(condition))                                          \
    {                                                         \
      set_assert_msg(assert_code, ASSERT_LINE, ASSERT_FILE);  \
    }                                                         \
  } while(0)

#else //ASSERT_ENABLE!=NO_TRACES

/** @brief Macro for calling an assert.
 *  @details This macro can be called by any firmware module in order to trigger an assertion.
 *  @param assert_code Error code to use for this assert.
 *  @param condition Test if false will trigger a call to the assert_handler() function.
 */
#define ASSERT(assert_code, condition)              do { } while(0)

/** @brief Macro for calling an assert from an interrupt.
 *  @details This macro can be called from an interrupt routine to set an assert message/code if the condition is false.
 *  @param assert_code Error code to use for this assert.
 *  @param condition Test if false will trigger a call to the assert_handler() function.
 */
#define ASSERT_MSG_IT(assert_code, condition)       do { } while(0)

/** @brief check assert macro.
 *  @details This macro checks if a call to ASSERT_MSG_IT has been made (calls check_assert() function).
 */
#define CHECK_ASSERT()                              do { } while(0)
#endif  //ASSERT_ENABLE!=NO_TRACES

#endif //ASSERT_H
/** @} */
