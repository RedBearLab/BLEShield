/* Copyright (c) 2011 Nordic Semiconductor. All Rights Reserved.
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
 * @brief memory library
*/

/** @defgroup lib_mem lib_mem
 * @{
 * @ingroup lib
 *  
 * @brief memory module
 * @details 
 * This library contains memory management functions.
*/

#ifndef LIB_MEM_H__
#define LIB_MEM_H__
#include "hal_platform.h"

/**@brief memory copy
 *  @details
 *  Call this function to copy the content of one buffer to another.
 *  @param dst : Pointer to the destination buffer.
 *  @param src : Pointer to the source buffer.
 *  @param length : Number of bytes to copy.
*/
void lib_mem_copy(uint8_t *dst, uint8_t *src, uint8_t length)KEYWORD_REENTRANT;

/** @} */

#endif

