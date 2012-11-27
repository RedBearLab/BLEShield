/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
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

#include "lib_aci.h"
//#include "lib_traces.h"
#include "app.h"

void radio_evt_error(void)
{
  lib_aci_error_descriptor_t error_description;

  lib_aci_interpret_error(&error_description);

  switch (error_description.error_code)
  {
  case ERROR_CMD_RESPONSE_ERROR :
//    LIB_TRACES_LOG_INFO_1VAR(INFO_CODE_ERR_HANDLING_SUBCODE1, error_description.error_sub_code1);
//    LIB_TRACES_LOG_INFO_1VAR(INFO_CODE_ERR_HANDLING_SUBCODE2, error_description.error_sub_code2);
    switch(error_description.error_sub_code2)
    {
    case ACI_STATUS_EXTENDED                      :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_EXTENDED);
      break;
    case ACI_STATUS_ERROR_UNKNOWN                 :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_UNKNOWN);
      break;
    case ACI_STATUS_ERROR_INTERNAL                :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_INTERNAL);
      break;
    case ACI_STATUS_ERROR_CMD_UNKNOWN             :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_CMD_UNKNOWN);
      break;
    case ACI_STATUS_ERROR_DEVICE_STATE_INVALID         :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_DEVICE_STATE_INVALID);
      break;
    case ACI_STATUS_ERROR_INVALID_LENGTH              :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_INVALID_LENGTH);
      break;
    case ACI_STATUS_ERROR_INVALID_PARAMETER           :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_INVALID_PARAMETER);
      break;
    case ACI_STATUS_ERROR_BUSY                    :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_BUSY);
      break;
    case ACI_STATUS_ERROR_INVALID_DATA            :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_INVALID_DATA);
      break;
    case ACI_STATUS_ERROR_CRC_MISMATCH            :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_CRC_MISMATCH);
      break;
    case ACI_STATUS_ERROR_UNSUPPORTED_SETUP_FORMAT:
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_UNSUPPORTED_SETUP_FORMAT);
      break;
    case ACI_STATUS_ERROR_INVALID_SEQ_NO          :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_INVALID_SEQ_NO);
      break;
    case ACI_STATUS_ERROR_SETUP_LOCKED            :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_SETUP_LOCKED);
      break;
    case ACI_STATUS_ERROR_LOCK_FAILED             :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_LOCK_FAILED);
      break;
    case ACI_STATUS_ERROR_BOND_REQUIRED           :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_BOND_REQUIRED);
      break;
    case ACI_STATUS_ERROR_REJECTED                  :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_REJECTED);
      break;
    case ACI_STATUS_ERROR_DATA_SIZE               :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_DATA_SIZE);
      break;
    case ACI_STATUS_ERROR_PIPE_INVALID            :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_PIPE_INVALID);
      break;
    case ACI_STATUS_ERROR_CREDIT_NOT_AVAILABLE    :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_CREDIT_NOT_AVAILABLE);
      break;
    case ACI_STATUS_ERROR_PEER_ATT_ERROR          :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_PEER_ATT_ERROR);
      break;
    case ACI_STATUS_ERROR_ADVT_TIMEOUT            :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_ADVT_TIMEOUT);
      break;
    case ACI_STATUS_ERROR_PEER_SMP_ERROR                           :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_PEER_SMP_ERROR);
      break;
    case ACI_STATUS_ERROR_PIPE_TYPE_INVALID                       :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_PIPE_TYPE_INVALID);
      break;
    case ACI_STATUS_ERROR_PIPE_STATE_INVALID                      :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_PIPE_STATE_INVALID);
      break;
    case ACI_STATUS_ERROR_INVALID_KEY_SIZE                        :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_INVALID_KEY_SIZE);
      break;
    case ACI_STATUS_ERROR_INVALID_KEY_DATA                        :
//      LIB_TRACES_LOG_ERROR(ERROR_CODE_ACI_STATUS_ERROR_INVALID_KEY_DATA);
      break;
    }
  break;
  case ERROR_RADIO_EVT_ERROR :
//    LIB_TRACES_LOG_ERROR(ERROR_CODE_ERR_HANDLING_RADIO_ERROR);
    break;
  case ERROR_RADIO_EVT_UNKNOWN :
//    LIB_TRACES_LOG_ERROR_1VAR(ERROR_CODE_ERR_HANDLING_RADIO_EVT_UNKNOWN, error_description.error_sub_code1);
    break;
  case ERROR_CMD_RESPONSE_WRONG_CMD :
//    LIB_TRACES_LOG_ERROR_1VAR(ERROR_CODE_ERR_HANDLING_RADIO_CMD_RESPONSE_WRONG_CMD_RCVD, error_description.error_sub_code1);
    //LIB_TRACES_LOG_ERROR_1VAR(ERROR_CODE_ERR_HANDLING_RADIO_CMD_RESPONSE_WRONG_CMD_EXPECTED, error_description.error_sub_code2);
    break;
  case ERROR_CMD_RESPONSE_TOO_SHORT :
//    LIB_TRACES_LOG_ERROR_1VAR(ERROR_CODE_ERR_HANDLING_RADIO_CMD_RESPONSE_TOO_SHORT, error_description.error_sub_code1);
    break;
  default:
//    LIB_TRACES_LOG_ERROR_1VAR(ERROR_CODE_ERR_HANDLING_ERROR_DEFAULT, error_description.error_code);
    /*Start the setup procedure again to try to recover*/
    app_state = APP_SETUP;
    on_process_app();
  }
}
