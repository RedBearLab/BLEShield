/*  Copyright (c) 2013, Nordic Semiconductor ASA
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *
 *  Redistributions in binary form must reproduce the above copyright notice, this
 *  list of conditions and the following disclaimer in the documentation and/or
 *  other materials provided with the distribution.
 *
 *  Neither the name of Nordic Semiconductor ASA nor the names of its
 *  contributors may be used to endorse or promote products derived from
 *  this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
  
#if defined(__PIC32MX__)
// neither PROGMEM or PSTR are needed for PIC32, just define them as null
   #define PROGMEM
   #define PSTR(s) (s)

   #define pgm_read_byte(x)	        (*((char *)x))
   #define pgm_read_byte_near(x)	(*((char *)x))
   #define pgm_read_byte_far(x)	    (*((char *)x))
   #define pgm_read_word(x)    	    (*((short *)x))
   #define pgm_read_word_near(x)	(*((short *)x))
   #define pgm_read_workd_far(x)	(*((short *)x))

   #define	prog_void	    const void
   #define	prog_char	    const char
   #define	prog_uchar	    const unsigned char
   #define	prog_int8_t	    const int8_t
   #define	prog_uint8_t	const uint8_t
   #define	prog_int16_t	const int16_t
   #define	prog_uint16_t	const uint16_t
   #define	prog_int32_t	const int32_t
   #define	prog_uint32_t	const uint32_t
   #define	prog_int64_t	const int64_t
   #define	prog_uint64_t	const uint64_t
#else
#include <avr/pgmspace.h>
#endif
#include <lib_aci.h>
#include "aci_setup.h"
#include "wiring.h"
#include "WProgram.h"

// aci_struct that will contain 
// total initial credits
// current credit
// current state of the aci (setup/standby/active/sleep)
// open remote pipe pending
// close remote pipe pending
// Current pipe available bitmap
// Current pipe closed bitmap
// Current connection interval, slave latency and link supervision timeout
// Current State of the the GATT client (Service Discovery status)


extern hal_aci_data_t msg_to_send;



/**************************************************************************                */
/* Utility function to fill the the ACI command queue                                      */
/* aci_stat               Pointer to the ACI state                                         */
/* num_cmd_offset(in/out) Offset in the Setup message array to start from                  */
/*                        offset is updated to the new index after the queue is filled     */
/*                        or the last message us placed in the queue                       */
/* Returns                void                                                             */
/***************************************************************************/
void aci_setup_fill(aci_state_t *aci_stat, uint8_t *num_cmd_offset)
{
    
  while (*num_cmd_offset < aci_stat->aci_setup_info.num_setup_msgs)
  {
    //Copy the setup ACI message
    //Add 2 bytes to the length byte for status byte, length for the total number of bytes
    memcpy(&msg_to_send, &(aci_stat->aci_setup_info.setup_msgs[*num_cmd_offset]), 
              (aci_stat->aci_setup_info.setup_msgs[*num_cmd_offset].buffer[0]+2)); 
    
    //Put the Setup ACI message in the command queue
    if (!hal_aci_tl_send(&msg_to_send))
    {
		//ACI Command Queue is full
		// *num_cmd_offset is now pointing to the index of the Setup command that did not get sent
		return;
    }
    
    (*num_cmd_offset)++;
  }
 
}

  

aci_status_code_t do_aci_setup(aci_state_t *aci_stat)
{
  uint8_t setup_offset         = 0;
  uint16_t i                   = 0x0000;
  aci_evt_t * aci_evt          = NULL;
  aci_status_code_t cmd_status = ACI_STATUS_ERROR_CRC_MISMATCH;
  
  /*
  We are using the same buffer since we are copying the contents of the buffer 
  when queuing and immediately processing the buffer when receiving
  */
  hal_aci_evt_t  *aci_data = (hal_aci_evt_t *)&msg_to_send;
  
 
  while (cmd_status != ACI_STATUS_TRANSACTION_COMPLETE)
  {	  
	if (setup_offset < aci_stat->aci_setup_info.num_setup_msgs)
	{
		aci_setup_fill(aci_stat,  &setup_offset);
	}

	i++; //i is used as a guard counter, if this counter overflows, there is an error	
	if (i > 0xFFFE)
	{
		return ACI_STATUS_ERROR_INTERNAL;	
	}
	
    if (true == lib_aci_event_get(aci_stat, aci_data))
    {
		  aci_evt    = &(aci_data->evt);

		  
		  if (ACI_EVT_CMD_RSP != aci_evt->evt_opcode )
		  {
			  //Got something other than a command response evt -> Error
			  return ACI_STATUS_ERROR_INTERNAL;
		  }
		        
          cmd_status = (aci_status_code_t) aci_evt->params.cmd_rsp.cmd_status;
		  switch (aci_evt->params.cmd_rsp.cmd_status)
		  {
			  case ACI_STATUS_TRANSACTION_CONTINUE:
			  //Go back to the the top of the loop so the queue can be filled up again
			  break;
			  
			  case ACI_STATUS_TRANSACTION_COMPLETE:
			  //Break out of the while loop when this status code appears
			  break;
			  
			  default:
			  //Any other status code is an error
			  return (aci_status_code_t )aci_evt->params.cmd_rsp.cmd_status;
			  break;			  
		  } 
	}
  }
  
  return (aci_status_code_t)aci_evt->params.cmd_rsp.cmd_status;
}
  
  

