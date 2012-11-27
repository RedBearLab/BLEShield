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

#ifndef INFO_CODES_H
#define INFO_CODES_H


/*log_info from lib_dispatcher*/            #define  INFO_CODE_LIB_DISPATCHER_INVALID_HANLER                          0x00 // handler invalid
/*log_info from error_handling*/            #define  INFO_CODE_ERR_HANDLING_SUBCODE1                                  0x01 // error_handling, error received error_sub_code1
                                            #define  INFO_CODE_ERR_HANDLING_SUBCODE2                                  0x02 // error_handling, error received error_sub_code2
/*log_info from application*/ /*common*/    #define  INFO_CODE_WKUP_BUTTON                                            0x03 // wakeup from deep sleep caused by button press
                                            #define  INFO_CODE_MAIN_STARTED                                           0x04 // main started
                                            #define  INFO_CODE_WAKEUP_RADIO                                           0x05 // send_wakeup command to the radio
                                            #define  INFO_CODE_DEEP_SLEEP                                             0x06 // go to deep_sleep
                                            #define  INFO_CODE_DEVICE_VERSION_HOST_BUILD_LSB                          0x07 // device_version : host_build LSB 
                                            #define  INFO_CODE_DEVICE_VERSION_HOST_BUILD_MSB                          0x08 // device_version : host_build MSB 
                                            #define  INFO_CODE_DEVICE_VERSION_ACI_VERSION                             0x09 // device_version : aci_version 
                                            #define  INFO_CODE_SETUP                                                  0x0A // radio in setup mode
                                            #define  INFO_CODE_DEVICE_RUN                                             0x0B // set_operation_mode run
                                            #define  INFO_CODE_CONNECT                                                0x0C // start advertising
                                            #define  INFO_CODE_BOND                                                   0x0D // start advertising to bond
                                            #define  INFO_CODE_CONNECTED                                              0x0E // connected
                                            #define  INFO_CODE_DISCONNECTED                                           0x0F // disconnected 
                                            #define  INFO_CODE_CONFIG_HOST                                            0x10 // proximity send host_configuration message
                                            #define  INFO_CODE_WKUP_TIMER                                             0x11 // wakeup from deep sleep caused by timer
                                            #define  INFO_CODE_WAIT_STD_BY                                            0x12 // proximity wait start std_by event
                                            #define  INFO_CODE_PIPE_UPDATED_PIPE_NUM                                  0x13 // pipes updated event for pipe num :
                                            #define  INFO_CODE_PIPE_UPDATED_DATA0                                     0x14 // pipes updated event data[0] :
                                            #define  INFO_CODE_STARTED_STDBY                                          0x15 // DeviceStartedEvent
                                            #define  INFO_CODE_TIMING_EVT                                             0x16 // received timing Event
                                            #define  INFO_CODE_DATA_ACK                                               0x17 // received data_ack Event from pipe
                                            #define  INFO_CODE_PIPE_STATUS_EVT                                        0x18 // received pipe_status Event
                                            #define  INFO_CODE_KEY_REQUEST                                            0x19 // received key request Event
                                            #define  INFO_CODE_DISPLAY_PASSKEY                                        0x1A // received display passkey Event
                                            #define  INFO_CODE_ADV_TIMEOUT                                            0x1B // advertising timed out
                                            #define  INFO_CODE_DEBUG_RC_EVT_CODE                                      0x1C // DEBUG_RC_EVT_CODE
                                            #define  INFO_CODE_DEBUG_RC_CMD_RSP_CODE                                  0x1D // DEBUG_RC_CMD_RSP_CODE
                                            #define  INFO_CODE_DATA_RECEIVED_LEN                                      0x1E // Length of data in DataReceivedEvent
                                            #define  INFO_CODE_CREDIT                                                 0x1F // Number of credits received
                                            #define  INFO_CODE_DEVICE_VERSION_CONFIG_ID                               0x20 // Config ID of the nRF8001


#endif //INFO_CODES_H 
/** @} */             































