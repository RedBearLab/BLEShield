/* Copyright (c) 2010 Nordic Semiconductor. All Rights Reserved.
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

/** @file
 * 
 */
#include "hal_platform.h"

#include "assertions.h"
#include "lib_traces.h"
#include "system.h"

#if ASSERT_ENABLE!=NO_TRACES
bool has_asserted = false;
#if ASSERT_ENABLE==CODED_TRACES
uint8_t memorized_assert_code;
#endif //ASSERT_ENABLE==CODED_TRACES
void check_assert()
{
  if (has_asserted)
  {
    LIB_TRACES_LOG_ERROR(memorized_assert_code);
    DISABLE_INTERRUPTS();
    while(1);
  }
}

void assert_handler(uint8_t assert_code, uint16_t line, char* file)
{
  line = line; file = file;
  LIB_TRACES_LOG_ERROR(assert_code);
}

void set_assert_msg(uint8_t assert_code, uint16_t line, char* file)
{
  line = line; file = file;
  has_asserted = true;
  memorized_assert_code = assert_code;
}

#endif //ASSERT_ENABLE!=NO_TRACES

/**
 * @}
 */

