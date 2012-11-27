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

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

/*error from hal_aci_tl*/         #define ERROR_CODE_HAL_ACI_TL_OVERFLOW                                   0x00  // given message length bigger than HAL_ACI_MAX_LENGTH
                                  #define ERROR_CODE_HAL_ACI_TL_STATUS_BYTE                                0x01  // Hardware Status Byte not 0 !
/*error from lib_aci*/            #define ERROR_CODE_LIB_ACI_SET_ADDRESS                                   0x02  // lib_aci_set_address
                                  #define ERROR_CODE_LIB_ACI_OVERFLOW                                      0x03  // lib_aci message length bigger than max buffer size
                                  #define ERROR_CODE_LIB_ACI_PIPE_OUT_OF_RANGE                             0x04  // pipe is bigger than max number of pipes
                                  #define ERROR_CODE_LIB_ACI_INVALID_PIPE_TYPE                             0x05  // pipe is not the correct pipe type for the command
                                  #define ERROR_CODE_LIB_ACI_SERVICE_DATA_PIPE_BITMAP_OUT_OF_RANGE         0x06  // Service data pipe bitmap out of range
                                  #define ERROR_CODE_LIB_ACI_BROADCAST_TIMEOUT_OUT_OF_RANGE                0x07  // Broadcast timeout value out of range
                                  #define ERROR_CODE_LIB_ACI_BROADCAST_ADV_INTERVAL_OUT_OF_RANGE           0x08  // Broadcast adv_interval out of range
/*error from lib_dispatcher*/     #define ERROR_CODE_LIB_DISPATCHER_INVALID_HANDLE                         0x09  // lib_dispatcher_invalid_handle
                                  #define ERROR_CODE_LIB_DISPATCHER_MSG_OVERFLOW                           0x0A  // lib_dispatcher_msg_overflow
                                  #define ERROR_CODE_LIB_DISPATCHER_DELAYED_MSG_OVERFLOW                   0x0B  // lib_dispatcher_delayed_msg_overflow
                                  #define ERROR_CODE_LIB_TIMED_SERVICE_DELAYED_QUEUE_CORRUPTED             0x0C  // lib_timed_service : queue_corrupted, impossible to add handler
/*error from error_handling*/     #define ERROR_CODE_ACI_STATUS_EXTENDED                                   0x0D  // Extended status, further checks needed
                                  #define ERROR_CODE_ACI_STATUS_ERROR_UNKNOWN                              0x0E  // Unknown error.
                                  #define ERROR_CODE_ACI_STATUS_ERROR_INTERNAL                             0x0F  // Internal error.
                                  #define ERROR_CODE_ACI_STATUS_ERROR_CMD_UNKNOWN                          0x10  // Unknown command
                                  #define ERROR_CODE_ACI_STATUS_ERROR_DEVICE_STATE_INVALID                 0x11  // Command invalid in the current device state
                                  #define ERROR_CODE_ACI_STATUS_ERROR_INVALID_LENGTH                       0x12  // Invalid length
                                  #define ERROR_CODE_ACI_STATUS_ERROR_INVALID_PARAMETER                    0x13  // Invalid input parameters
                                  #define ERROR_CODE_ACI_STATUS_ERROR_BUSY                                 0x14  // Busy 
                                  #define ERROR_CODE_ACI_STATUS_ERROR_INVALID_DATA                         0x15  // Invalid data format or contents
                                  #define ERROR_CODE_ACI_STATUS_ERROR_CRC_MISMATCH                         0x16  // CRC mismatch
                                  #define ERROR_CODE_ACI_STATUS_ERROR_UNSUPPORTED_SETUP_FORMAT             0x17  // Unsupported setup format
                                  #define ERROR_CODE_ACI_STATUS_ERROR_INVALID_SEQ_NO                       0x18  // Invalid sequence number during a write dynamic data sequence
                                  #define ERROR_CODE_ACI_STATUS_ERROR_SETUP_LOCKED                         0x19  // Setup data is locked and cannot be modified
                                  #define ERROR_CODE_ACI_STATUS_ERROR_LOCK_FAILED                          0x1A  // Setup error due to lock verification failure
                                  #define ERROR_CODE_ACI_STATUS_ERROR_BOND_REQUIRED                        0x1B  // Bond required: Local Pipes need bonded/trusted peer
                                  #define ERROR_CODE_ACI_STATUS_ERROR_REJECTED                             0x1C  // Command rejected as a transaction is still pending
                                  #define ERROR_CODE_ACI_STATUS_ERROR_DATA_SIZE                            0x1D  // Pipe Error Event : Data size exceeds size specified for pipe : Transmit failed
                                  #define ERROR_CODE_ACI_STATUS_ERROR_PIPE_INVALID                         0x1E  // Pipe Error Event : Transmit failed, Invalid or unavailable Pipe number or unknown pipe type, no credit will be used, no credit returned
                                  #define ERROR_CODE_ACI_STATUS_ERROR_CREDIT_NOT_AVAILABLE                 0x1F  // Pipe Error Event : Credit not available
                                  #define ERROR_CODE_ACI_STATUS_ERROR_PEER_ATT_ERROR                       0x20  // Pipe Error Event : Peer device has sent an error on an pipe operation on the remote characteristic
                                  #define ERROR_CODE_ACI_STATUS_ERROR_ADVT_TIMEOUT                         0x21  // Connection was not established before the BTLE advertising was stopped
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_EVT_UNKNOWN                        0x22  // error handling : radio event unknown
                                  #define ERROR_CODE_ERR_HANDLING_ERROR_DEFAULT                            0x23  // error_code_err_handling_error_default
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_ERROR                              0x24  // error_code_err_handling_radio_error
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_HW_ERROR                           0x25  // radio hardware error
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_HW_ERROR_LINE_NUM_MSB              0x26  // radio hardware error line num (MSB) 
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_HW_ERROR_LINE_NUM_LSB              0x27  // radio hardware error line num (LSB) 
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_HW_ERROR_CONTENT                   0x28  // radio hardware error name of file  
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_CMD_RESPONSE_WRONG_CMD_RCVD        0x29  // received a command response to a different command than the last sent command : resp to cmd
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_CMD_RESPONSE_WRONG_CMD_EXPECTED    0x2A  // received a command response to a different command than the last sent command : last sent cmd
/*error from application*/        #define ERROR_CODE_DISCONNECT_NULL_DATA_PTR                              0x2B  // disconnect_null_data_ptr
                                  #define ERROR_CODE_STDBY_EVT_NOT_EXPECTED                                0x2C  // stdby event received in state
                                  #define ERROR_CODE_STDBY_EVT_WITH_HW_ERROR                               0x2D  // stdby event received with debug info 
                                  #define ERROR_CODE_SETUP_ERROR                                           0x2E  // all set_up messages sent but SETUP_COMPLETE not received
                                  #define ERROR_CODE_ERR_HANDLING_RADIO_CMD_RESPONSE_TOO_SHORT             0x2F  // command response event received with length too short to be interpreted
                                  #define ERROR_CODE_DEV_STARTED_EVT_UNKNOWN_STATE                         0x30  // device started event received in unknown state
                                  #define ERROR_CODE_PIPE_ERROR                                            0x31  // Received a pipe ERROR on pipe 
                                  #define ERROR_CODE_UNEXPECTED_STARTED_TEST_EVT                           0x32  // received a device started event in test mode when not expected
                                  #define ERROR_CODE_ACI_STATUS_ERROR_PEER_SMP_ERROR                       0x33  // Peer has triggered a Security Manager Protocol Error
                                  #define ERROR_CODE_ACI_STATUS_ERROR_PIPE_TYPE_INVALID                    0x34  // Pipe Error Event : Pipe type invalid for the selected operation
                                  #define ERROR_CODE_ACI_STATUS_ERROR_PIPE_STATE_INVALID                   0x35  // Pipe Error Event : Pipe state invalid for the selected operation
                                  #define ERROR_CODE_ACI_STATUS_ERROR_INVALID_KEY_SIZE                     0x36  // Invalid key size provided
                                  #define ERROR_CODE_ACI_STATUS_ERROR_INVALID_KEY_DATA                     0x37  // Invalid key data provided

#endif //ERROR_CODES_H                                                                                        
/** @} */                                                                                                     
                                                                                                              
                                                                                                              
                                                                                                              








































