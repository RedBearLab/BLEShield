/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 4808 $
 */ 
/* Attention! 
*  To maintain compliance with Nordic Semiconductor ASA�s Bluetooth profile 
*  qualification listings, this section of source code must not be modified.
*/

/** @file
  @brief Implementation of the ACI library.
 */

#include "hal_platform.h"
#include "aci.h"
#include "aci_cmds.h"
#include "aci_evts.h"
#include "aci_protocol_defines.h"
#include "acilib_defs.h"
#include "acilib_if.h"
#include "services.h"
#include "hal_power.h"
#include "hal_aci_tl.h"
#include "lib_aci.h"
#include "assertions.h"
#include "lib_mem.h"

#ifdef SETUP_MESSAGES_CONTENT
static hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] = SETUP_MESSAGES_CONTENT;
#else
static hal_aci_data_t setup_msgs[1] = {{0x00,{0x00}}};
#endif

static radio_setup_state_t host_setup_state;

static hal_aci_data_t  msg_to_send;
static bool is_send_buffer_free = true;
static hal_aci_data_t  *p_rcvd_evt;

#define LIB_ACI_DEFAULT_CREDIT_NUMBER 1

#define PIPES_ARRAY_SIZE      ((ACI_DEVICE_MAX_PIPES + 7)/8)

void hal_aci_tl_msg_rcv_hook(hal_aci_data_t *p_rcvd_data);

static aci_cmd_opcode_t cur_transaction_cmd;
static aci_cmd_opcode_t memorized_rcvd_cmd_opcode;
static aci_cmd_opcode_t memorized_transaction_cmd_opcode;
static uint8_t memorized_rcvd_cmdrsp_status;


static bool is_cmd_response_expected;
static lib_aci_cb_id device_mode2handle(const aci_evt_params_device_started_t *p_device_started_data);

static uint16_t cx_rf_interval;  // rf_interval = cx_rf_interval * 1.25 ms Range:0x0006 to 0x0C80
static uint16_t current_slave_latency;
static uint8_t total_nb_available_credits;
static uint8_t nb_available_credits;

static bool is_request_operation_pending, is_indicate_operation_pending, is_open_remote_pipe_pending, is_close_remote_pipe_pending;
static uint8_t request_operation_pipe = 0;
static uint8_t indicate_operation_pipe = 0;

static uint8_t pipes_open_bitmap[PIPES_ARRAY_SIZE];
static uint8_t pipes_closed_bitmap[PIPES_ARRAY_SIZE];
static uint8_t adv_pipes_bitmap[PIPES_ARRAY_SIZE] = {0};
static uint8_t cur_error_code;

bool lib_aci_is_pipe_available(uint8_t pipe)
{
  uint8_t byte_idx;
#ifdef NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE,(pipe <= NUMBER_OF_PIPES)); 
#endif // NUMBER_OF_PIPES
  byte_idx = pipe / 8;
  if (pipes_open_bitmap[byte_idx] & (0x01 << (pipe % 8)))
  {
    return(true);
  }
  return(false);
}


bool lib_aci_is_pipe_closed(uint8_t pipe)
{
  uint8_t byte_idx;
#ifdef NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE,(pipe <= NUMBER_OF_PIPES)); 
#endif // NUMBER_OF_PIPES
  byte_idx = pipe / 8;
  if (pipes_closed_bitmap[byte_idx] & (0x01 << (pipe % 8)))
  {
    return(true);
  }
  return(false);
}


bool lib_aci_is_discovery_finished()
{
  return(pipes_open_bitmap[0]&0x01);
}


bool lib_aci_is_host_setup_complete()
{
  return(RADIO_SETUP_COMPLETE == host_setup_state);
}


void lib_aci_init()
{
  uint8_t i;

  hal_aci_tl_init();

  for (i = 0; i < PIPES_ARRAY_SIZE; i++)
  {
    pipes_open_bitmap[i]   = 0;
    pipes_closed_bitmap[i] = 0;
    adv_pipes_bitmap[i]    = 0;
  }
  
  nb_available_credits             = LIB_ACI_DEFAULT_CREDIT_NUMBER;
  total_nb_available_credits       = LIB_ACI_DEFAULT_CREDIT_NUMBER;
  is_cmd_response_expected         = true;
  is_send_buffer_free              = true;
  is_request_operation_pending     = false;
  is_indicate_operation_pending    = false; 
  is_open_remote_pipe_pending      = false;
  is_close_remote_pipe_pending     = false;
  cur_transaction_cmd              = ACI_CMD_INVALID;
  memorized_rcvd_cmd_opcode        = ACI_CMD_INVALID;
  memorized_transaction_cmd_opcode = ACI_CMD_INVALID;
  cx_rf_interval                   = 0;
  current_slave_latency            = 0;
  request_operation_pipe           = 0;
  indicate_operation_pipe          = 0;
  cur_error_code                   = ERROR_RADIO_EVT_ERROR;
  p_rcvd_evt                       = __null;
}


uint8_t lib_aci_get_nb_available_credits()
{
  return nb_available_credits;
}

uint16_t lib_aci_get_cx_interval_ms()
{
  uint32_t cx_rf_interval_ms_32bits;
  uint16_t cx_rf_interval_ms;
  cx_rf_interval_ms_32bits = cx_rf_interval;
  cx_rf_interval_ms_32bits *= 125;                      // the connection interval is given in multiple of 0.125 milliseconds
  cx_rf_interval_ms = cx_rf_interval_ms_32bits / 100;
  return cx_rf_interval_ms;
}


uint16_t lib_aci_get_cx_interval()
{
  return cx_rf_interval;
}


uint16_t lib_aci_get_slave_latency()
{
  return current_slave_latency;
}


bool lib_aci_set_application_latency(uint16_t latency, aci_app_latency_mode_t latency_mode)
{
  bool ret_val = false;
  aci_cmd_params_set_app_latency_t aci_set_app_latency;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    is_cmd_response_expected = true;
    aci_set_app_latency.mode = latency_mode;
    aci_set_app_latency.latency = latency;
    acil_encode_cmd_set_app_latency(&(msg_to_send.buffer[0]), &aci_set_app_latency);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else 
    {
      cur_transaction_cmd = ACI_CMD_SET_APP_LATENCY;
    }
  }
  return ret_val;
}


bool lib_aci_set_test_mode(aci_test_mode_change_t enter_exit_test_mode)
{
  bool ret_val = false;
  aci_cmd_params_test_t aci_cmd_params_test;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    aci_cmd_params_test.test_mode_change = enter_exit_test_mode;
    acil_encode_cmd_set_test_mode(&(msg_to_send.buffer[0]), &aci_cmd_params_test);
    is_cmd_response_expected = false;
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_TEST;
    }
  }
  return ret_val;
}


bool lib_aci_sleep()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_sleep(&(msg_to_send.buffer[0]));
    is_cmd_response_expected = false;
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
  }
  return ret_val;
}


bool lib_aci_radio_reset()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_baseband_reset(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_RADIO_RESET;
    }
  }
  return ret_val;
}


bool lib_aci_direct_connect()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_direct_connect(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_CONNECT_DIRECT;
    }
  }
  return ret_val;
}


bool lib_aci_device_version()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_get_device_version(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_GET_DEVICE_VERSION;
    }
   
  }
  return ret_val;
}


bool lib_aci_set_local_data(uint8_t pipe, uint8_t *p_value, uint8_t size)
{
  bool ret_val = false;
  aci_cmd_params_set_local_data_t aci_cmd_params_set_local_data;

  ASSERT(ERROR_CODE_LIB_ACI_OVERFLOW,(size <= ACI_PIPE_TX_DATA_MAX_LEN));
#ifdef NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE,(pipe <= NUMBER_OF_PIPES)); 
  ASSERT(ERROR_CODE_LIB_ACI_INVALID_PIPE_TYPE,(services_pipe_type_mapping[pipe-1].location == ACI_STORE_LOCAL));
#endif // NUMBER_OF_PIPES
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    if (size > ACI_PIPE_TX_DATA_MAX_LEN)
    {
      size = ACI_PIPE_TX_DATA_MAX_LEN;
    }
    aci_cmd_params_set_local_data.tx_data.pipe_number = pipe;
    lib_mem_copy(&(aci_cmd_params_set_local_data.tx_data.aci_data[0]), p_value, size);
    acil_encode_cmd_set_local_data(&(msg_to_send.buffer[0]), &aci_cmd_params_set_local_data, size);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_SET_LOCAL_DATA;
    }
  }
  return ret_val;
}


bool lib_aci_send_setup_msg()
{
#ifdef SETUP_MESSAGES_CONTENT
  static uint8_t nb_setup_msg_sent = 0;
  if (is_send_buffer_free)
  {
    if (0 == nb_setup_msg_sent)
    {
      host_setup_state = RADIO_SETUP_IN_PROGRESS;
    }
    if (RADIO_SETUP_FAILED == host_setup_state)
    {
      nb_setup_msg_sent = NB_SETUP_MESSAGES;// setup has failed
      lib_aci_transaction_finished_hook();
      return(true);
    }
    is_send_buffer_free = false;
    if (hal_aci_tl_send(&(setup_msgs[nb_setup_msg_sent])))
    {
      cur_transaction_cmd = (aci_cmd_opcode_t)ACIL_DECODE_EVT_GET_OPCODE(&(setup_msgs[nb_setup_msg_sent].buffer[0]));
      nb_setup_msg_sent++;
      if (nb_setup_msg_sent >= NB_SETUP_MESSAGES)
      {
        nb_setup_msg_sent = 0;
        return(true);
      }
    }
    else
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
  }
#endif
  return (false);
}


bool lib_aci_connect(uint16_t run_timeout, uint16_t adv_interval)
{
  bool ret_val = false;
  aci_cmd_params_connect_t aci_cmd_params_connect;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    aci_cmd_params_connect.timeout = run_timeout;
    aci_cmd_params_connect.adv_interval = adv_interval;
    acil_encode_cmd_connect(&(msg_to_send.buffer[0]), &aci_cmd_params_connect);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_CONNECT;
    }
  }
  return ret_val;
}


bool lib_aci_disconnect(aci_disconnect_reason_t reason)
{
  bool ret_val = false;
  aci_cmd_params_disconnect_t aci_cmd_params_disconnect;
  uint8_t i = 0;
  if (is_send_buffer_free)
  {
  
    is_send_buffer_free = false;
    aci_cmd_params_disconnect.reason = reason;
    acil_encode_cmd_disconnect(&(msg_to_send.buffer[0]), &aci_cmd_params_disconnect);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      // Update pipes immediately so that while the disconnect is happening,
      // the application cant attempt sending another message 
      for (i=0; i < PIPES_ARRAY_SIZE; i++)
      {
        pipes_open_bitmap[i] = 0;
        pipes_closed_bitmap[i] = 0;
      }
      cur_transaction_cmd = ACI_CMD_DISCONNECT;
    }
  }
  return ret_val;
}


bool lib_aci_bond(uint16_t run_timeout, uint16_t adv_interval)
{
  bool ret_val = false;
  aci_cmd_params_bond_t aci_cmd_params_bond;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    aci_cmd_params_bond.timeout = run_timeout;
    aci_cmd_params_bond.adv_interval = adv_interval;
    acil_encode_cmd_bond(&(msg_to_send.buffer[0]), &aci_cmd_params_bond);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_BOND;
    }
  }
  return ret_val;
}


bool lib_aci_device_wakeup()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_wakeup(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_WAKEUP;
    }
  }
  return ret_val;
}


bool lib_aci_set_radio_tx_power(aci_device_output_power_t tx_power)
{
  bool ret_val = false;
  aci_cmd_params_set_tx_power_t aci_cmd_params_set_tx_power;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    aci_cmd_params_set_tx_power.device_power = tx_power;
    acil_encode_cmd_set_radio_tx_power(&(msg_to_send.buffer[0]), &aci_cmd_params_set_tx_power);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_SET_TX_POWER;
    }
  }
  return ret_val;
}


bool lib_aci_get_address()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_get_address(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_GET_DEVICE_ADDRESS;
    }
  }
  return ret_val;
}


bool lib_aci_get_temperature()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_temparature(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_GET_TEMPERATURE;
    }
  }
  return ret_val;
}


bool lib_aci_get_battery_level()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_battery_level(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; //  Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_GET_BATTERY_LEVEL;
    }
  }
  return ret_val;
}


bool lib_aci_send_data(uint8_t pipe, uint8_t *p_value, uint8_t size)
{
  bool ret_val = false;

  aci_cmd_params_send_data_t aci_cmd_params_send_data;
#ifdef NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE,(pipe <= NUMBER_OF_PIPES)); 
  ASSERT(ERROR_CODE_LIB_ACI_INVALID_PIPE_TYPE,((services_pipe_type_mapping[pipe-1].pipe_type == ACI_TX)||(services_pipe_type_mapping[pipe-1].pipe_type == ACI_TX_ACK)||(services_pipe_type_mapping[pipe-1].pipe_type == ACI_TX_SIGN)));  
#endif // NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_OVERFLOW,(size <= ACI_PIPE_TX_DATA_MAX_LEN));
  if (size > ACI_PIPE_TX_DATA_MAX_LEN)
  {
    size = ACI_PIPE_TX_DATA_MAX_LEN;
  }
  if ((nb_available_credits > 0) && !is_request_operation_pending)
  {
    if (is_send_buffer_free)
    {
#ifdef NUMBER_OF_PIPES
      if (services_pipe_type_mapping[pipe-1].location == ACI_STORE_LOCAL &&
          services_pipe_type_mapping[pipe-1].pipe_type == ACI_TX_ACK)
      {
        if (!is_indicate_operation_pending)
        {
          is_indicate_operation_pending = true;
          indicate_operation_pipe = pipe;
        }
        else
        {
          return (false);
        }
      }
#endif //NUMBER_OF_PIPES
      is_send_buffer_free = false;
      nb_available_credits--;
      aci_cmd_params_send_data.tx_data.pipe_number = pipe;
      lib_mem_copy(&(aci_cmd_params_send_data.tx_data.aci_data[0]), p_value, size);
      acil_encode_cmd_send_data(&(msg_to_send.buffer[0]), &aci_cmd_params_send_data, size);
      is_cmd_response_expected = false;
      ret_val = hal_aci_tl_send(&msg_to_send);
      if (!ret_val)
      {
        is_send_buffer_free = true; // Use the buffer for the next command 
        nb_available_credits++;
      }
    }
  }
  return ret_val;
}


bool lib_aci_request_data(uint8_t pipe)
{
  bool ret_val = false;
  aci_cmd_params_request_data_t aci_cmd_params_request_data;
#ifdef NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE,(pipe <= NUMBER_OF_PIPES)); 
  ASSERT(ERROR_CODE_LIB_ACI_INVALID_PIPE_TYPE,((services_pipe_type_mapping[pipe-1].location == ACI_STORE_REMOTE)&&(services_pipe_type_mapping[pipe-1].pipe_type == ACI_RX_REQ)));
#endif //NUMBER_OF_PIPES
  if ((nb_available_credits > 0)&&(!is_request_operation_pending))
  {
    if (is_send_buffer_free)
    {
      nb_available_credits--;
      is_request_operation_pending = true;
      request_operation_pipe = pipe;
      is_send_buffer_free = false;
      aci_cmd_params_request_data.pipe_number = pipe;
      acil_encode_cmd_request_data(&(msg_to_send.buffer[0]), &aci_cmd_params_request_data);
      is_cmd_response_expected = false;
      ret_val = hal_aci_tl_send(&msg_to_send);
      if (!ret_val)
      {
        is_send_buffer_free = true;
        nb_available_credits++;
        is_request_operation_pending = false;
      }
      else
      {
        cur_transaction_cmd = ACI_CMD_REQUEST_DATA;
      }
    }
  }
  return ret_val;
}


bool lib_aci_change_timing(uint16_t minimun_cx_interval, uint16_t maximum_cx_interval, uint16_t slave_latency, uint16_t timeout)
{
  bool ret_val = false;
  aci_cmd_params_change_timing_t aci_cmd_params_change_timing;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    aci_cmd_params_change_timing.conn_params.min_conn_interval = minimun_cx_interval;
    aci_cmd_params_change_timing.conn_params.max_conn_interval = maximum_cx_interval;
    aci_cmd_params_change_timing.conn_params.slave_latency     = slave_latency;    
    aci_cmd_params_change_timing.conn_params.timeout_mult      = timeout;     
    acil_encode_cmd_change_timing_req(&(msg_to_send.buffer[0]), &aci_cmd_params_change_timing);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; // Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_CHANGE_TIMING;
    }
  }
  return ret_val;
}


bool lib_aci_change_timing_GAP_PPCP()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;     
    acil_encode_cmd_change_timing_req_GAP_PPCP(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; // Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_CHANGE_TIMING;
    }
  }
  return ret_val;
}


bool lib_aci_open_remote_pipe(uint8_t pipe)
{
  bool ret_val = false;
  aci_cmd_params_open_remote_pipe_t aci_cmd_params_open_remote_pipe;
#ifdef NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE,(pipe <= NUMBER_OF_PIPES));
  ASSERT(ERROR_CODE_LIB_ACI_INVALID_PIPE_TYPE,((services_pipe_type_mapping[pipe-1].location == ACI_STORE_REMOTE)&&((services_pipe_type_mapping[pipe-1].pipe_type == ACI_RX)||(services_pipe_type_mapping[pipe-1].pipe_type == ACI_RX_ACK_AUTO)||(services_pipe_type_mapping[pipe-1].pipe_type == ACI_RX_ACK))));  
#endif //NUMBER_OF_PIPES
  if ((is_send_buffer_free)&&(!is_request_operation_pending))
  {
    is_send_buffer_free = false;
    is_request_operation_pending = true;
    is_open_remote_pipe_pending = true;
    request_operation_pipe = pipe;
    aci_cmd_params_open_remote_pipe.pipe_number = pipe;
    acil_encode_cmd_open_remote_pipe(&(msg_to_send.buffer[0]), &aci_cmd_params_open_remote_pipe);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; // Use the buffer for the next command 
      is_request_operation_pending = false;
      is_open_remote_pipe_pending = false;
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_OPEN_REMOTE_PIPE;
    }
  }
  return ret_val;
}


bool lib_aci_close_remote_pipe(uint8_t pipe)
{
  bool ret_val = false;
  aci_cmd_params_close_remote_pipe_t aci_cmd_params_close_remote_pipe;
#ifdef NUMBER_OF_PIPES
  ASSERT(ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE,(pipe <= NUMBER_OF_PIPES));
  ASSERT(ERROR_CODE_LIB_ACI_INVALID_PIPE_TYPE,((services_pipe_type_mapping[pipe-1].location == ACI_STORE_REMOTE)&&((services_pipe_type_mapping[pipe-1].pipe_type == ACI_RX)||(services_pipe_type_mapping[pipe-1].pipe_type == ACI_RX_ACK_AUTO)||(services_pipe_type_mapping[pipe-1].pipe_type == ACI_RX_ACK))));  
#endif //NUMBER_OF_PIPES
  if ((is_send_buffer_free)&&(!is_request_operation_pending))
  {
    is_send_buffer_free = false;
    is_request_operation_pending = true;
    is_close_remote_pipe_pending = true;
    request_operation_pipe = pipe;
    aci_cmd_params_close_remote_pipe.pipe_number = pipe;
    acil_encode_cmd_close_remote_pipe(&(msg_to_send.buffer[0]), &aci_cmd_params_close_remote_pipe);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; // Use the buffer for the next command 
      is_request_operation_pending = false;
      is_close_remote_pipe_pending = false;
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_CLOSE_REMOTE_PIPE;
    }
  }
  return ret_val;
}


bool lib_aci_set_key(aci_key_type_t key_rsp_type, uint8_t *key, uint8_t len)
{
  bool ret_val = false;
  aci_cmd_params_set_key_t aci_cmd_params_set_key;

  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    aci_cmd_params_set_key.key_type = key_rsp_type;
    lib_mem_copy((uint8_t*)&(aci_cmd_params_set_key.key), key, len);
    acil_encode_cmd_set_key(&(msg_to_send.buffer[0]), &aci_cmd_params_set_key);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; // Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_SET_KEY;
    }
  }
  return ret_val;
}


bool lib_aci_echo_msg(uint8_t msg_size, uint8_t *p_msg_data)
{
  bool ret_val = false;
  aci_cmd_params_echo_t aci_cmd_params_echo;
  ASSERT(ERROR_CODE_LIB_ACI_OVERFLOW,(msg_size <= (ACI_ECHO_DATA_MAX_LEN)));
  if (is_send_buffer_free)
  {
    is_cmd_response_expected = false;
    is_send_buffer_free = false;
    if (msg_size > (ACI_ECHO_DATA_MAX_LEN))
    {
      msg_size = ACI_ECHO_DATA_MAX_LEN;
    }

    lib_mem_copy(&(aci_cmd_params_echo.echo_data[0]), p_msg_data, msg_size);
    acil_encode_cmd_echo_msg(&(msg_to_send.buffer[0]), &aci_cmd_params_echo, msg_size);

    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; // Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_ECHO;
    }
  }
  return ret_val;
}


bool lib_aci_bond_request()
{
  bool ret_val = false;
  if (is_send_buffer_free)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_bond_security_request(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true; // Use the buffer for the next command 
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_BOND_SECURITY_REQUEST;
    }
  }
  return ret_val;
}

static lib_aci_cb_id device_mode2handle(const aci_evt_params_device_started_t *p_device_started_data)
{
  switch (p_device_started_data->device_mode)
  {
    case ACI_DEVICE_SETUP :
      return(LIB_ACI_STARTED_SETUP);
    case ACI_DEVICE_TEST :
      return(LIB_ACI_STARTED_TEST);
    case ACI_DEVICE_STANDBY :
      return(LIB_ACI_STARTED_STDBY);
  }
  return(LIB_ACI_RADIO_STARTED_UNKNOWN_STATE);
}


void lib_aci_interpret_error(lib_aci_error_descriptor_t *p_error_data)
{
  p_error_data->error_code = cur_error_code;
  switch(cur_error_code)
  {
    case ERROR_CMD_RESPONSE_ERROR:
      p_error_data->error_sub_code1 = memorized_transaction_cmd_opcode;
      p_error_data->error_sub_code2 = memorized_rcvd_cmdrsp_status;
      break;
    case ERROR_CMD_RESPONSE_WRONG_CMD:
      p_error_data->error_sub_code1 = memorized_rcvd_cmd_opcode;
      p_error_data->error_sub_code2 = memorized_transaction_cmd_opcode;
      break;
    case ERROR_RADIO_EVT_UNKNOWN:
      p_error_data->error_sub_code1 = ACIL_DECODE_EVT_GET_OPCODE(&(p_rcvd_evt->buffer[0]));
      break;
    case ERROR_CMD_RESPONSE_TOO_SHORT :
      p_error_data->error_sub_code1 = ACIL_DECODE_EVT_GET_LENGTH(&(p_rcvd_evt->buffer[0]));
      break;
    case ERROR_RADIO_EVT_ERROR:
    default :
      break;
  }
}

void hal_aci_tl_msg_rcv_hook(hal_aci_data_t *p_rcvd_data)
{
  lib_aci_cb_id callback_id = LIB_ACI_DEFAULT;
  bool is_call_transact_finished = false;
  aci_evt_t rcvd_evt;
  uint8_t data_len;

  if (!is_cmd_response_expected)
  {
    is_send_buffer_free = true;
    is_call_transact_finished = true;
  }
  is_cmd_response_expected = true;
  rcvd_evt.len = ACIL_DECODE_EVT_GET_LENGTH(&(p_rcvd_data->buffer[0]));
  data_len = 0;
  if (rcvd_evt.len > 0)
  {
    rcvd_evt.evt_opcode = (aci_evt_opcode_t)ACIL_DECODE_EVT_GET_OPCODE(&(p_rcvd_data->buffer[0]));
    p_rcvd_evt = p_rcvd_data;
    switch(rcvd_evt.evt_opcode)
    {
      case ACI_EVT_CMD_RSP:
        {
          is_send_buffer_free = true;
          is_call_transact_finished = true;
          if (rcvd_evt.len < EVT_CMD_RESPONSE_MIN_LENGTH)
          {
            cur_error_code = ERROR_CMD_RESPONSE_TOO_SHORT;
            callback_id = LIB_ACI_ERROR;
          }
          else
          {
            acil_decode_evt_command_response(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.cmd_rsp));
            callback_id = LIB_ACI_CMD_RESPONSE;

            if ((ACI_STATUS_SUCCESS != rcvd_evt.params.cmd_rsp.cmd_status)&&(ACI_CMD_WAKEUP != rcvd_evt.params.cmd_rsp.cmd_opcode))
            {
              if (ACI_STATUS_TRANSACTION_CONTINUE == rcvd_evt.params.cmd_rsp.cmd_status)
              {
                if (RADIO_SETUP_FAILED != host_setup_state)
                {
                  host_setup_state   = RADIO_SETUP_IN_PROGRESS;
                }
              }
              else if (ACI_STATUS_TRANSACTION_COMPLETE == rcvd_evt.params.cmd_rsp.cmd_status)
              {
                if (RADIO_SETUP_FAILED != host_setup_state)
                {
                  host_setup_state   = RADIO_SETUP_COMPLETE;
                }
              }
              else
              {
                if (ACI_CMD_OPEN_REMOTE_PIPE == rcvd_evt.params.cmd_rsp.cmd_opcode)
                {
                  is_request_operation_pending = false;
                  is_open_remote_pipe_pending = false;
                }
                if (ACI_CMD_CLOSE_REMOTE_PIPE == rcvd_evt.params.cmd_rsp.cmd_opcode)
                {
                  is_request_operation_pending = false;
                  is_close_remote_pipe_pending = false;
                }
                cur_error_code = ERROR_CMD_RESPONSE_ERROR;
                callback_id = LIB_ACI_ERROR;
                memorized_rcvd_cmd_opcode = rcvd_evt.params.cmd_rsp.cmd_opcode;
                memorized_rcvd_cmdrsp_status = rcvd_evt.params.cmd_rsp.cmd_status;
                if ((RADIO_SETUP_IN_PROGRESS == host_setup_state))
                {
                  host_setup_state = RADIO_SETUP_FAILED;
                }
              }
            }
            if (ACI_CMD_GET_DEVICE_VERSION == rcvd_evt.params.cmd_rsp.cmd_opcode)
            {
              callback_id = LIB_ACI_DEVICE_VERSION;
            }
            if (cur_transaction_cmd != rcvd_evt.params.cmd_rsp.cmd_opcode)
            {
              memorized_transaction_cmd_opcode = cur_transaction_cmd;
              memorized_rcvd_cmd_opcode = rcvd_evt.params.cmd_rsp.cmd_opcode;
              cur_error_code = ERROR_CMD_RESPONSE_WRONG_CMD;
              callback_id = LIB_ACI_ERROR;
            }
          }
        }
        break;
      case ACI_EVT_DEVICE_STARTED:
        {
          acil_decode_evt_device_started(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.device_started));
          callback_id = device_mode2handle(&(rcvd_evt.params.device_started));
          nb_available_credits = rcvd_evt.params.device_started.credit_available;
          total_nb_available_credits = rcvd_evt.params.device_started.credit_available;
          if(ACI_DEVICE_SETUP == rcvd_evt.params.device_started.device_mode)
          {
            host_setup_state = RADIO_SETUP_NOT_STARTED;
          }
        }
        break;
      case ACI_EVT_DATA_RECEIVED:
        {
          data_len = acil_decode_evt_data_received(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.data_received));
          if (rcvd_evt.params.data_received.rx_data.pipe_number == request_operation_pipe)
          {
            is_request_operation_pending = false;
          }
          callback_id = LIB_ACI_DATA_RCVD;
        }
        break;
      case ACI_EVT_PIPE_STATUS:
        {
          uint8_t i;

          acil_decode_evt_pipe_status(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.pipe_status));
          for (i=0; i < PIPES_ARRAY_SIZE; i++)
          {
            pipes_open_bitmap[i] = rcvd_evt.params.pipe_status.pipes_open_bitmap[i];
            pipes_closed_bitmap[i] = rcvd_evt.params.pipe_status.pipes_closed_bitmap[i];
          }
          if (is_open_remote_pipe_pending)
          {
            if (lib_aci_is_pipe_available(request_operation_pipe))
            {
              is_open_remote_pipe_pending = false;
              is_request_operation_pending = false;
            }
          }
          if (is_close_remote_pipe_pending)
          {
            if (!lib_aci_is_pipe_available(request_operation_pipe))
            {
              is_close_remote_pipe_pending = false;
              is_request_operation_pending = false;
            }
          }
          callback_id = LIB_ACI_PIPE_STATUS;
        }
        break;
      case ACI_EVT_DISCONNECTED:
        {
          uint8_t i;
          acil_decode_evt_disconnected(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.disconnected));
          for (i=0; i < PIPES_ARRAY_SIZE; i++)
          {
            pipes_open_bitmap[i] = 0;
            pipes_closed_bitmap[i] = 0;
          }
          nb_available_credits = total_nb_available_credits;
          if ((ACI_STATUS_ERROR_ADVT_TIMEOUT == rcvd_evt.params.disconnected.aci_status) || 
                ((ACI_STATUS_EXTENDED == rcvd_evt.params.disconnected.aci_status)&& (0x3C == rcvd_evt.params.disconnected.btle_status)))  // TBD create defines for 0x3C and all BT error codes
          {
            callback_id = LIB_ACI_ADV_TIMEOUT;
          }
          else
          {
            callback_id = LIB_ACI_DISCONNECT;
          }
        }
        break;
      case ACI_EVT_DATA_CREDIT:
        {
          acil_decode_evt_credit(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.data_credit));
          nb_available_credits += rcvd_evt.params.data_credit.credit;
          callback_id = LIB_ACI_CREDIT;
        }
        break;
      case ACI_EVT_TIMING:
        {
          acil_decode_evt_timing(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.timing));
          cx_rf_interval = rcvd_evt.params.timing.conn_rf_interval;
          current_slave_latency = rcvd_evt.params.timing.conn_slave_rf_latency;
          callback_id = LIB_ACI_TIMING;
        }
        break;
      case ACI_EVT_CONNECTED:
        {
          acil_decode_evt_connected(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.connected));
          cx_rf_interval = rcvd_evt.params.connected.conn_rf_interval;
          current_slave_latency = rcvd_evt.params.connected.conn_slave_rf_latency;
          is_request_operation_pending = false;
          is_open_remote_pipe_pending = false;
          is_open_remote_pipe_pending = false;
          is_indicate_operation_pending = false;
          callback_id = LIB_ACI_CONNECTED;
          nb_available_credits = total_nb_available_credits;
        }
        break;
      case ACI_EVT_BOND_STATUS:
        {
          acil_decode_evt_bond_status(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.bond_status));
          callback_id = LIB_ACI_BOND_STATUS;
        }
        break;
      case ACI_EVT_HW_ERROR:
        {
          data_len = acil_decode_evt_hw_error(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.hw_error));
          callback_id = LIB_ACI_HW_ERROR;
        }
        break;
      case ACI_EVT_PIPE_ERROR :
        {
          acil_decode_evt_pipe_error(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.pipe_error));
          if (rcvd_evt.params.pipe_error.pipe_number == request_operation_pipe)
          {
            is_open_remote_pipe_pending = false;
            is_request_operation_pending = false;
          }
          else if (rcvd_evt.params.pipe_error.pipe_number == indicate_operation_pipe)
          {
            is_indicate_operation_pending = false;
          }
          if (ACI_STATUS_ERROR_PEER_ATT_ERROR != rcvd_evt.params.pipe_error.error_code)
          {
            nb_available_credits ++;
            if (nb_available_credits > total_nb_available_credits)
            {
              nb_available_credits = total_nb_available_credits;
            }
          }
          callback_id = LIB_ACI_PIPE_ERROR;
        }
        break;
      case ACI_EVT_ECHO :
        acil_decode_evt_echo(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.echo));
        callback_id = LIB_ACI_EVT_ECHO;
        break;
      case ACI_EVT_DATA_ACK :
        {
          acil_decode_evt_data_ack(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.data_ack));
          if (rcvd_evt.params.data_ack.pipe_number == request_operation_pipe)
          {
            is_request_operation_pending = false;
          }
          else if (rcvd_evt.params.data_ack.pipe_number == indicate_operation_pipe)
          {
            is_indicate_operation_pending = false;
          }
          callback_id = LIB_ACI_DATA_ACK;
        }
        break;
      case ACI_EVT_KEY_REQUEST :
        {
          acil_decode_evt_key_request(&(p_rcvd_evt->buffer[0]), &(rcvd_evt.params.key_request));
          callback_id = LIB_ACI_KEY_REQUEST;
        }
        break;
      case ACI_EVT_DISPLAY_PASSKEY:
        acil_decode_evt_display_passkey(&(p_rcvd_evt->buffer[0]),  &(rcvd_evt.params.display_passkey));
        callback_id = LIB_ACI_DISPLAY_PASSKEY;
        break;
      default:
        cur_error_code =  ERROR_RADIO_EVT_UNKNOWN;
        callback_id = LIB_ACI_ERROR;
        break;
    }
  }
    
  if (is_call_transact_finished)
  {
    lib_aci_transaction_finished_hook();
  }
  switch(callback_id)
  {
    case LIB_ACI_STARTED_SETUP:
      lib_aci_started_setup_hook(&rcvd_evt);
      break;
    case LIB_ACI_STARTED_TEST :
      lib_aci_started_test_hook(&rcvd_evt);
      break;
    case LIB_ACI_STARTED_STDBY :
      lib_aci_started_stdby_hook(&rcvd_evt);
      break;
    case LIB_ACI_DEVICE_VERSION :
      lib_aci_device_version_hook(&rcvd_evt);
      break;
    case LIB_ACI_CMD_RESPONSE :
      lib_aci_cmd_response_hook(&rcvd_evt);
      break;
    case LIB_ACI_DATA_RCVD :
      lib_aci_data_rcvd_hook(&rcvd_evt, data_len);
      break;
    case LIB_ACI_PIPE_STATUS :
      lib_aci_pipe_status_hook(&rcvd_evt);
      break;
    case LIB_ACI_ADV_TIMEOUT :
      lib_aci_adv_timeout_hook(&rcvd_evt);
      break;
    case LIB_ACI_DISCONNECT :
      lib_aci_disconnect_hook(&rcvd_evt);
      break;
    case LIB_ACI_CREDIT :
      lib_aci_credit_hook(&rcvd_evt);
      break;
    case LIB_ACI_TIMING :
      lib_aci_timing_hook(&rcvd_evt);
      break;
    case LIB_ACI_CONNECTED :
      lib_aci_connected_hook(&rcvd_evt);
      break;
    case LIB_ACI_BOND_STATUS :
      lib_aci_bond_status_hook(&rcvd_evt);
      break;
    case LIB_ACI_HW_ERROR :
      lib_aci_hw_error_hook(&rcvd_evt, data_len);
      break;
    case LIB_ACI_PIPE_ERROR :
      lib_aci_pipe_error_hook(&rcvd_evt);
      break;
    case LIB_ACI_DATA_ACK :
      lib_aci_data_ack_hook(&rcvd_evt);
      break;
    case LIB_ACI_KEY_REQUEST :
      lib_aci_key_request_hook(&rcvd_evt);
      break;
    case LIB_ACI_DISPLAY_PASSKEY :
      lib_aci_display_passkey_hook(&rcvd_evt);
      break;
    case LIB_ACI_EVT_ECHO :
      lib_aci_evt_echo_hook(&rcvd_evt);
      break;
    case LIB_ACI_ERROR :
      lib_aci_error_hook();
      break;
    case LIB_ACI_RADIO_STARTED_UNKNOWN_STATE :
      lib_aci_error_hook();
      break;
  }
}


bool lib_aci_send_ack(const uint8_t pipe)
{
  bool ret_val = false;
  if ( (nb_available_credits > 0) && (is_send_buffer_free == true))
  {
    is_send_buffer_free = false;
    nb_available_credits--;
    acil_encode_cmd_send_data_ack(&(msg_to_send.buffer[0]), pipe);
    is_cmd_response_expected = false;
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (ret_val == false)
    {
      is_send_buffer_free = true; // Allow other commands to go through though this command failed
      nb_available_credits++;
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_SEND_DATA_ACK;
    }
  }
  return ret_val;
}


bool lib_aci_send_nack(const uint8_t pipe, const uint8_t error_code)
{
  bool ret_val = false;
  if ( (nb_available_credits > 0) && (is_send_buffer_free == true))
  {
    is_send_buffer_free = false;
    nb_available_credits--;
    is_cmd_response_expected = false;
    acil_encode_cmd_send_data_nack(&(msg_to_send.buffer[0]), pipe, error_code);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (ret_val == false)
    {
      is_send_buffer_free = true; // Allow other commands to go through though this command failed
      nb_available_credits++;
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_SEND_DATA_NACK;
    }
  }
  return ret_val;
}


bool lib_aci_broadcast(const uint16_t timeout, const uint16_t adv_interval)
{
  bool ret_val = false;
  aci_cmd_params_broadcast_t aci_cmd_params_broadcast;
  ASSERT(ERROR_CODE_LIB_ACI_BROADCAST_TIMEOUT_OUT_OF_RANGE,(timeout <= 16383)); 
  ASSERT(ERROR_CODE_LIB_ACI_BROADCAST_ADV_INTERVAL_OUT_OF_RANGE,(32 <= adv_interval && adv_interval <= 16384)); 

  if (is_send_buffer_free == true)
  {
    is_send_buffer_free = false;
    aci_cmd_params_broadcast.timeout = timeout;
    aci_cmd_params_broadcast.adv_interval = adv_interval;
    acil_encode_cmd_broadcast(&(msg_to_send.buffer[0]), &aci_cmd_params_broadcast);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true;  // Use the buffer for the next command
      
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_BROADCAST;  /* Only if the aci_tl_send was successful, 
                                                   can we know for sure that it is the 
                                                   current transaction is for this command */
    }
  }
  return ret_val;
}


bool lib_aci_open_adv_pipes(const uint8_t * const adv_service_data_pipes)
{
  bool ret_val = false;
  int i = 0;
  aci_cmd_params_open_adv_pipe_t aci_cmd_params_open_adv_pipe;
  ASSERT(ERROR_CODE_LIB_ACI_SERVICE_DATA_PIPE_BITMAP_OUT_OF_RANGE,(((adv_service_data_pipes[0] & 0x01) == 0) && ((adv_service_data_pipes[7] & 0x80) == 0))); 
  
  if (is_send_buffer_free == true)
  {
    is_send_buffer_free = false;
    for (i = 0; i < 8; i++)
    {
      aci_cmd_params_open_adv_pipe.pipes[i] = adv_service_data_pipes[i];
    }

    acil_encode_cmd_open_adv_pipes(&(msg_to_send.buffer[0]), &aci_cmd_params_open_adv_pipe);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true;  // Use the buffer for the next command
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_OPEN_ADV_PIPE;  // Only if the aci_tl_send was successful, 
                                                    // can we know for sure that it is the 
                                                    // current transaction is for this command 
    }
  }
  return ret_val;
}

bool lib_aci_open_adv_pipe(const uint8_t pipe)
{
  bool ret_val = false;
  int i = 0;
  aci_cmd_params_open_adv_pipe_t aci_cmd_params_open_adv_pipe;
  uint8_t byte_idx = pipe / 8;

  adv_pipes_bitmap[byte_idx] |= (0x01 << (pipe % 8));

  ASSERT(ERROR_CODE_LIB_ACI_SERVICE_DATA_PIPE_BITMAP_OUT_OF_RANGE,(((adv_pipes_bitmap[0] & 0x01) == 0) && ((adv_pipes_bitmap[7] & 0x80) == 0)));

  if (is_send_buffer_free == true)
  {
    is_send_buffer_free = false;
    for (i = 0; i < 8; i++)
    {
      aci_cmd_params_open_adv_pipe.pipes[i] = adv_pipes_bitmap[i];
    }

    acil_encode_cmd_open_adv_pipes(&(msg_to_send.buffer[0]), &aci_cmd_params_open_adv_pipe);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true;  // Use the buffer for the next command

    }
    else
    {
      cur_transaction_cmd = ACI_CMD_OPEN_ADV_PIPE;  /* Only if the aci_tl_send was successful, 
                                                   can we know for sure that it is the 
                                                   current transaction is for this command */
    }
  }
  return ret_val;
}


bool lib_aci_read_dynamic_data()
{
  bool ret_val = false;
  
  if (is_send_buffer_free == true)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_read_dynamic_data(&(msg_to_send.buffer[0]));
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true;  // Use the buffer for the next command
      
    }
    else
    {
      cur_transaction_cmd = ACI_CMD_READ_DYNAMIC_DATA;  /* Only if the aci_tl_send was successful, 
                                                   can we know for sure that it is the 
                                                   current transaction is for this command */
      is_cmd_response_expected = true;
    }
  }
  return ret_val;
}


bool lib_aci_write_dynamic_data(uint8_t sequence_number, uint8_t* dynamic_data, uint8_t length)
{
  bool ret_val = false;

  if (is_send_buffer_free == true)
  {
    is_send_buffer_free = false;
    acil_encode_cmd_write_dynamic_data(&(msg_to_send.buffer[0]), sequence_number, dynamic_data, length);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true;  // Use the buffer for the next command

    }
    else
    {
      cur_transaction_cmd = ACI_CMD_WRITE_DYNAMIC_DATA;  /* Only if the aci_tl_send was successful,
                                                   can we know for sure that it is the
                                                   current transaction is for this command */
      is_cmd_response_expected = true;
    }
  }
  return ret_val;
}

bool lib_aci_dtm_command(uint8_t dtm_command_msbyte, uint8_t dtm_command_lsbyte)
{
  bool ret_val = false;

  if (is_send_buffer_free == true)
  {
    aci_cmd_params_dtm_cmd_t aci_cmd_params_dtm_cmd;

    is_send_buffer_free = false;
    aci_cmd_params_dtm_cmd.cmd_msb = dtm_command_msbyte;
    aci_cmd_params_dtm_cmd.cmd_lsb = dtm_command_lsbyte;
    acil_encode_cmd_dtm_cmd(&(msg_to_send.buffer[0]), &aci_cmd_params_dtm_cmd);
    ret_val = hal_aci_tl_send(&msg_to_send);
    if (!ret_val)
    {
      is_send_buffer_free = true;  // Use the buffer for the next command

    }
    else
    {
      cur_transaction_cmd = ACI_CMD_DTM_CMD;  /* Only if the aci_tl_send was successful,
                                                   can we know for sure that it is the
                                                   current transaction is for this command */
      is_cmd_response_expected = true;
    }
  }
  return ret_val;
}


/** @} */
