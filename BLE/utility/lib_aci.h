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
 * $LastChangedRevision$
 */
/* Attention! 
*  To maintain compliance with Nordic Semiconductor ASA’s Bluetooth profile 
*  qualification listings, this section of source code must not be modified.
*/

#ifndef LIB_ACI_H__
#define LIB_ACI_H__

/** @file
* @brief ACI library
*/

/** @addtogroup lib_aci
@{
@brief Library for the logical part of the Application Controller Interface (ACI)
*/


#include "hal_platform.h"
#include "hal_aci_tl.h"
#include "aci.h"
#include "aci_cmds.h"
#include "aci_evts.h"

#define ERROR_CMD_RESPONSE_ERROR 0xb2
#define ERROR_RADIO_EVT_ERROR 0xc3
#define ERROR_RADIO_EVT_UNKNOWN 0xd4
#define ERROR_CMD_RESPONSE_WRONG_CMD 0xd5
#define ERROR_CMD_RESPONSE_TOO_SHORT 0xd8

#define EVT_CMD_RESPONSE_MIN_LENGTH              3


typedef enum radio_setup_state_t{
  RADIO_SETUP_NOT_STARTED,
  RADIO_SETUP_IN_PROGRESS,
  RADIO_SETUP_COMPLETE,
  RADIO_SETUP_FAILED,
}radio_setup_state_t;


typedef struct lib_aci_error_descriptor_t {
  uint8_t error_code;
  uint8_t error_sub_code1;
  uint8_t error_sub_code2;
} lib_aci_error_descriptor_t;

typedef enum lib_aci_cb_id{
  LIB_ACI_STARTED_SETUP = 0,
  LIB_ACI_STARTED_TEST,
  LIB_ACI_STARTED_STDBY,
  LIB_ACI_DEVICE_VERSION,
  LIB_ACI_CMD_RESPONSE,
  LIB_ACI_DATA_RCVD,
  LIB_ACI_PIPE_STATUS,
  LIB_ACI_ADV_TIMEOUT,
  LIB_ACI_DISCONNECT,
  LIB_ACI_CREDIT,
  LIB_ACI_TIMING,
  LIB_ACI_CONNECTED,
  LIB_ACI_BOND_STATUS,
  LIB_ACI_HW_ERROR,
  LIB_ACI_PIPE_ERROR,
  LIB_ACI_DATA_ACK,
  LIB_ACI_KEY_REQUEST,
  LIB_ACI_DISPLAY_PASSKEY,
  LIB_ACI_EVT_ECHO,
  LIB_ACI_ERROR,
  LIB_ACI_RADIO_STARTED_UNKNOWN_STATE,
  LIB_ACI_DEFAULT,
  LIB_ACI_READ_DYNAMIC_DATA
}lib_aci_cb_id;

#define DISCONNECT_REASON_CX_TIMEOUT                 0x08
#define DISCONNECT_REASON_CX_CLOSED_BY_PEER_DEVICE   0x13
#define DISCONNECT_REASON_POWER_LOSS                 0x14
#define DISCONNECT_REASON_CX_CLOSED_BY_LOCAL_DEVICE  0x16
#define DISCONNECT_REASON_ADVERTISER_TIMEOUT         0x50


void authenticate(aci_evt_params_data_received_t *p_data_rcvd_data, uint8_t data_len);


/** @brief Hook function for when an error occurs in lib_aci.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time an error occurs in lib_aci.
 */
extern void lib_aci_error_hook(void);

/** @brief Hook function for each time a transmit transaction on the ACI is finished.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a transaction towards nRF8001 is finished. 
 *  This tells your application that the ACI is now available for the next transaction.
 */
extern void lib_aci_transaction_finished_hook(void);

/** @brief Hook function for when a @c deviceStartedEvent in setup mode is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c deviceStartedEvent in setup mode is 
 *  received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_started_setup_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c deviceStartedEvent in test mode is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c deviceStartedEvent in test mode is 
 *  received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_started_test_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c deviceStartedEvent in standby mode is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c deviceStartedEvent in standby mode is 
 *  received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_started_stdby_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c CommandResponseEvent to a @c GetDeviceVersion command is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c CommandResponseEvent to a @c GetDeviceVersion 
 *  command is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_device_version_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c CommandResponseEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time @c CommandResponseEvent command is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_cmd_response_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c dataReceivedEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c dataReceivedEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 *  @param data_len Actual length of p_rcvd_data->params.data_received.rx_data.aci_data[].
 */
extern void lib_aci_data_rcvd_hook(aci_evt_t *p_rcvd_data, uint8_t data_len);

/** @brief Hook function for when a @c PipeStatusEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c PipeStatusEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_pipe_status_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c DisconnectedEvent with advTimeout reason is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c DisconnectedEvent with advTimeout reason
 *  is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_adv_timeout_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c DisconnectedEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c DisconnectedEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_disconnect_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c CreditEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c CreditEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_credit_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c TimingEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c TimingEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_timing_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c ConnectedEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c ConnectedEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_connected_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c BondStatusEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time is a @c BondStatusEvent received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_bond_status_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c HardwareErrorEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c HardwareErrorEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 *  @param data_len Actual length of p_rcvd_data->params.hw_error.file_name[].
*/
extern void lib_aci_hw_error_hook(aci_evt_t *p_rcvd_data, uint8_t data_len);

/** @brief Hook function for when a @c PipeErrorEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time is a @c PipeErrorEvent received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_pipe_error_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c DataAckEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c DataAckEvent is received on the ACI.
 */
extern void lib_aci_data_ack_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c KeyRequestEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c KeyRequestEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_key_request_hook(aci_evt_t *p_rcvd_data);

/** @brief Hook function for when a @c DisplayPassKeyEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time a @c DisplayPassKeyEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received data.
 */
extern void lib_aci_display_passkey_hook(aci_evt_t *p_rcvd_data);


/** @brief Hook function for when an @c EchoEvent is received.
 *  @details A hook function that must be implemented by the client of this module.
 *  The function is called each time an @c EchoEvent is received on the ACI.
 *  @param p_rcvd_data Pointer to the received Echo Event.
 */
extern void lib_aci_evt_echo_hook(aci_evt_t *p_rcvd_data);

/** @name Functions for library management */
//@{

/** @brief Initialization function.
 */
void lib_aci_init(void);

/** @brief Checks if the host setup is complete.
 *  @return True if the setup is complete, false if the status is continue.
 */
bool lib_aci_is_host_setup_complete(void);

/** @brief Gets the number of currently available ACI credits.
 *  @return Number of ACI credits.
 */
uint8_t lib_aci_get_nb_available_credits(void);

/** @brief Gets the connection interval in milliseconds.
 *  @return Connection interval in milliseconds.
 */
uint16_t lib_aci_get_cx_interval_ms(void);

/** @brief Gets the connection interval in multiple of 1.25&nbsp;ms.
 *  @return Connection interval in multiple of 1.25&nbsp;ms.
 */
uint16_t lib_aci_get_cx_interval(void);

/** @brief Gets the current slave latency.
 *  @return Current slave latency.
 */
uint16_t lib_aci_get_slave_latency(void);

/** @brief Checks if a given pipe is available.
 *  @param pipe Pipe to check.
 *  @return True if the pipe is available, otherwise false.
 */
bool lib_aci_is_pipe_available(uint8_t pipe);

/** @brief Checks if a given pipe is closed.
 *  @param pipe Pipe to check.
 *  @return True if the pipe is closed, otherwise false.
 */
bool lib_aci_is_pipe_closed(uint8_t pipe);

/** @brief Checks if the discovery operation is finished.
 *  @return True if the discovery is finished.
 */
bool lib_aci_is_discovery_finished(void);


/** @brief Retrieves the details associated to an error.
 *  @details If the lib_aci calls @ref lib_aci_error_hook, the hook function
 *  uses this to get more information about the error.
 *  @param[out] p_error_data Pointer to a lib_aci_error_descriptor_t structure.
 */
void lib_aci_interpret_error(lib_aci_error_descriptor_t *p_error_data);


//@}

/** @name ACI Commands available in all modes */
//@{

/** @brief Sets the radio in sleep mode.
 *  @details The function sends a @c sleep command to the radio. The radio is
 *  in sleep mode when lib_aci calls the @ref lib_aci_transaction_finished_hook() function.
 *  If the radio is advertising or connected, it sends back an error, then use lib_aci_radio_reset 
 *  if advertising or disconnect if in a connection.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_sleep(void);

/** @brief Resets the radio.
 *  @details The function sends a @c BasebandReset command to the radio. The radio is
 *  forced into idle state when lib_aci calls the @ref lib_aci_transaction_finished_hook() function.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_radio_reset(void);

/** @brief Radio starts directed advertising to bonded device.
 *  @details The function sends a @c DirectedConnect command to the radio. When the radio starts
 *  directed advertising,  lib_aci calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_direct_connect(void);

/** @brief Gets the radio's version.
 *  @details This function sends a @c GetDeviceVersion command. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the version is received.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_device_version(void);

/** @brief Gets the device address.
 *  @details This function sends a @c GetDeviceAddress command. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the device address is received.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_get_address(void);

/** @brief Gets the temperature.
 *  @details This function sends a @c GetTemperature command. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the temperature is received.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_get_temperature(void);

/** @brief Gets the battery level.
 *  @details This function sends a @c GetBatteryLevel command. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the battery level is received.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_get_battery_level(void);

//@}

/** @name ACI commands available in Sleep mode */
//@{

/** @brief Wakes up the radio.
 *  @details This function sends a @c Wakeup command to wake up the radio from
 *  sleep mode. When woken up the radio sends a @c DeviceStartedEvent and
 *  a @c CommandResponseEvent. lib_aci calls hooks for these two events,
 *  and calls @ref lib_aci_transaction_finished_hook.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_device_wakeup(void);

//@}

/** @name ACI commands available in Active mode */
//@{

/** @brief Sets the radio in test mode.
 *  @details This function sends a @c Test command to the radio. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the command is sent. There are two
 *  Test modes available:
 *  - UART: DTM commands are received over UART.
 *  - ACI: DTM commands are received over ACI.
 *  The same command is used to exit the test mode When receiving
 *  a @c DeviceStartedEvent the radio has entered the new mode.
 *  @param enter_exit_test_mode Enter a Test mode, or exit Test mode.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_set_test_mode(aci_test_mode_change_t enter_exit_test_mode);

/** @brief Sets the radio's TX power.
 *  @details This function sends a @c SetTxPower command. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is received.
 *  @param tx_power TX power to be used by the radio.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_set_radio_tx_power(aci_device_output_power_t tx_power);

/** @brief Tries to connect to a peer device.
 *  @details This function sends a @c Connect command to the radio. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received.
 *  @param run_timeout Maximum advertising time in seconds (0 means infinite).
 *  @param adv_interval Advertising interval (in multiple of 0.625&nbsp;ms).
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_connect(uint16_t run_timeout, uint16_t adv_interval);

/** @brief Tries to bond with a peer device.
 *  @details This function sends a @c Bond command to the radio. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received.
 *  @param run_timeout Maximum advertising time in seconds (0 means infinite).
 *  @param adv_interval Advertising interval (in multiple of 0.625&nbsp;ms).
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_bond(uint16_t run_timeout, uint16_t adv_interval);

/** @brief Disconnects from peer device.
 *  @details This function sends a @c Disconnect command to the radio. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received.
 *  @param reason Reason for disconnecting.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_disconnect(aci_disconnect_reason_t reason);

/**@brief Sets Local Data.
 *  @details
 *  This command updates the value of the characteristic value or the characteristic descriptor stored locally on the device.
 *  Can be called for all types of pipes as long as the data is stored locally.
 *
 *  @param pipe Pipe number on which the data should be set.
 *  @param value Pointer to the data to set.
 *  @param size Size of the data to set.
 *  @return True if the transaction is successfully initiated.
*/
bool lib_aci_set_local_data(uint8_t pipe, uint8_t *value, uint8_t size);
/** @brief Sends Broadcast message to the radio.
 *  @details The Broadcast message starts advertisement procedure 
 *  using the given interval with the intention of broadcasting data to a peer device.
 *  @param timeout Time, in seconds, to advertise before exiting to standby mode (0 means infinite).
 *  Valid values: 0 to 16383.
 *  @param adv_interval Advertising interval (in multiple of 0.625&nbsp;ms).
 *  Valid values: 32 to 16384.
 *  @return True if the broadcast message is sent successfully to the radio. 
*/
bool lib_aci_broadcast(const uint16_t timeout, const uint16_t adv_interval);

/** @name Open Advertising Pipes.  */

/** @brief Sends a command to the radio to set the input pipe to be placed in Advertisement Service Data.
 *  @details This function sends a command to the radio that places the pipe in 
 *  advertisement service data.  To start advertising service data, call this function before
 *  Connecting, Broadcasting or Bonding to peer. The data that should be sent in the advertisement packets 
 *  must be set using the @c lib_aci_set_local_data function. This function can be called during 
 *  advertising to enable/disable broadcast pipes.  
 *  @param pipe The pipe that has to be placed in advertising service data.
 *  @return True if the Open Adv Pipe message is sent successfully to the radio. 
*/
bool lib_aci_open_adv_pipe(const uint8_t pipe);


/** @name Open Advertising Pipes  */

/** @brief Sends a command to the radio to set the pipes to be placed in Advertisement Service Data.
 *  @details This function will send a command to the radio that will set the pipes to be placed in 
 *  advertisement Service Data.  To start advertising service data, this function should be called before
 *  Connecting, Broadcasting or Bonding to peer. This function can be called during 
 *  advertising to enable/disable broadcast pipes. Use this as an alternative to @ref lib_aci_open_adv_pipe 
 *  to avoid multiple function calls for placing multiple pipes in the adv data.
 *  @param adv_service_data_pipes Pipe bitmap, where '1' indicates that the corresponding 
 *  Valid Values: 0000000000000000 to FEFFFFFFFFFFFF7F
 *  TX_BROADCAST pipe data is to be placed in Advertising Service Data fields
 *  @return true if the Open Adv Pipe message was sent successfully to the radio. 
*/
bool lib_aci_open_adv_pipes(const uint8_t * const adv_service_data_pipes);


//@}

/** @name ACI commands available in Connected mode */
//@{


/** @brief Sets a given application latency.
 *  @details This function sends a @c setApplicationLatency command. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_set_application_latency(uint16_t latency, aci_app_latency_mode_t latency_mode);

/** @brief Opens a remote pipe.
 *  @details This function sends an @c OpenRemotePipe command. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received.
 *  @param pipe Number of the pipe to open.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_open_remote_pipe(uint8_t pipe);

/** @brief Closes a remote pipe.
 *  @details This function sends an @c CloseRemotePipe command. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is received.
 *  @param pipe Pipe number to close.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_close_remote_pipe(uint8_t pipe);

/** @brief Sends data on a given pipe.
 *  @details This function sends a @c SendData command with application data to
 *  the radio. lib_aci calls the @ref lib_aci_transaction_finished_hook() function when the command
 *  is sent. This function memorizes credit use, and checks that
 *  enough credits are available.
 *  @param pipe Pipe number on which the data should be sent.
 *  @param value Pointer to the data to send.
 *  @param size Size of the data to send.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_send_data(uint8_t pipe, uint8_t *value, uint8_t size);

/** @brief Requests data from a given pipe.
 *  @details This function sends a @c RequestData command to the radio. lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the command is sent. This
 *  function memorizes credit uses, and check that enough credits are available.
 *  After this command, the radio sends back either a @c DataReceivedEvent
 *  or a @c PipeErrorEvent.
 *  @param pipe Pipe number on which the data is requested.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_request_data(uint8_t pipe);

/** @brief Sends a L2CAP change connection parameters request.
 *  @details This function sends a @c ChangeTiming command to the radio. Lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received. This command triggers a "L2CAP change connection parameters" request 
 *  to the master. If the master rejects or accepts but doesn't change the connection parameters within
 *  30 seconds, a timing event with the unchanged connection parameters is sent by the radio.
 *  If the request is accepted and the master changes connection parameters, a timing event with
 *  the new connection parameters is sent by the radio.
 *  If the master doesn't reply to the request within 60 seconds, the radio disconnects.
 *  @param minimun_cx_interval Minimum connection interval requested, in multiple of 1.25&nbsp;ms.
 *  @param maximum_cx_interval Maximum connection interval requested, in multiple of 1.25&nbsp;ms.
 *  @param slave_latency requested slave latency.
 *  @param timeout requested slave timeout, in multiple of 10&nbsp;ms.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_change_timing(uint16_t minimun_cx_interval, uint16_t maximum_cx_interval, uint16_t slave_latency, uint16_t timeout);

/** @brief Sends a L2CAP change connection parameters request with the connection predefined preffered connection parameters.
 *  @details This function sends a @c ChangeTiming command to the radio. Lib_aci 
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is
 *  received. This command triggers a "L2CAP change connection parameters" request 
 *  to the master. If the master rejects or accepts but doesn't change the connection parameters within
 *  30 seconds, a timing event with the unchanged connection parameters is sent by the radio.
 *  If the request is accepted and the master changes connection parameters, a timing event with
 *  the new connection parameters is sent by the radio.
 *  If the master doesn't reply to the request within 60 seconds, the radio disconnects.
 *  The timing parameters used are the Timing parameters in the GAP settings in the nRFgo Studio.
 *  The Timing parameters as stored as the GAP Preferred Peripheral Connection Parameters.
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_change_timing_GAP_PPCP(void);

/** @brief Sends acknowledgement message to peer.
 *  @details This function sends @c SendDataAck command to radio. The radio is expected 
 *  to send either Handle Value Confirmation or Write response depending
 *  on whether the data is stored remotely or locally.
 *  @param pipe Pipe number for which the acknowledgement is to be sent.
 *  @return True if the ack was sent successfully. False otherwise.
*/
bool lib_aci_send_ack(const uint8_t pipe);

/** @brief Sends negative acknowledgement message to peer.
 *  @details This function sends @c SendDataNack command to radio. The radio is expected 
 *  to send Error Response to the peer.
 *  @param pipe Pipe number for which the nack is to be sent.
 *  @param error_code Error code to be sent in the NACk.
 *  @return True if the nack was sent successfully. False otherwise.
*/
bool lib_aci_send_nack(const uint8_t pipe, const uint8_t error_code);

/** @brief Sends ReadDynamicData command to the host. 
 *  @details This function sends @c ReadDynamicData command to host. The host is expected 
 *  to send @c CommandResponse back with the dynamic data. The application is expected to 
 *  call this function in a loop until all the dynamic data is read out from the host. 
 *  As long as there is dynamic data to be read from the host, the command response
 *  for this message has its status field set to ACI_STATUS_TRANSACTION_CONTINUE (0x01).
 *  The application may chose to store this read out data in a non-volatile memory location
 *  and later chose to write it back using the function lib_aci_write_dynamic_data.
 *  @return True if the command was sent successfully through the ACI. False otherwise.
*/
bool lib_aci_read_dynamic_data(void);

/** @brief Sends WriteDynamicData command to the host.
 *  @details This function sends @c WriteDynamicData command to host. The host is expected
 *  to send @c CommandResponse with the status of this operation. As long as the status field 
 *  in the @c CommandResponse is ACI_STATUS_TRANSACTION_CONTINUE (0x01), the hosts expects
 *  more dynamic data to be written. This function should ideally be called in a cycle, 
 *  until all the stored dynamic data is sent to the host. This function should be 
 *  called with the dynamic data obtained from the response to a @c ReadDynamicData 
 *  (see @c lib_aci_read_dynamic_data) command.
 *  @param sequence_number Sequence number of the dynamic data to be sent.
 *  @param dynamic_data Pointer to the dynamic data.
 *  @param length Length of the dynamic data.
 *  @return True if the command was sent successfully through the ACI. False otherwise.
*/
bool lib_aci_write_dynamic_data(uint8_t sequence_number, uint8_t* dynamic_data, uint8_t length);
//@}

/** @name ACI commands available while connected in Bond mode */
//@{

/** @brief Sends a SMP Security Request.
 *  @details This function send a @c BondRequest command to the radio. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the @c CommandResponseEvent is received.
 *  This command triggers a SMP Security Request to the master. If the
 *  master rejects with a pairing failed or if the bond timer expires the connection is closed. 
 *  @return True if the transaction is successfully initiated.
 */
bool lib_aci_bond_request(void);

/** @brief Set the key requested by the 8001. 
 *  @details This function sends an @c SetKey command to the radio. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the command is sent.
 *  @param key_rsp_type Type of key.
 *  @param key Pointer to the key to set.
 *  @param len Length of the key.
 *  @return True if the transaction is successfully initiated.
*/
bool lib_aci_set_key(aci_key_type_t key_rsp_type, uint8_t *key, uint8_t len);

//@}

/** @name ACI commands available in Setup mode */
//@{

/** @brief Sends the generated configuration messages
 *  @details This function sends a generated configuration message. The messages
 *  are defined in services.h (one for each project) which is included by
 *  lib_aci.c. lib_aci calls the @ref lib_aci_transaction_finished_hook() function when the
 *  @c CommandResponseEvent is received. This function must be called until it
 *  returns true. Once all the messages are sent, the radio automatically goes
 *   in to active mode.
 *  @return State of the Setup procedure.
 *  @retval True The setup is complete.
 *  @retval False Call the function again to continue.
 */
bool lib_aci_send_setup_msg(void);

//@}

/** @name ACI commands available in Test mode */
//@{

/** @brief Sends an echo message
 *  @details This function sends an @c Echo command to the radio. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the command is sent.
 *  @param message_size Length of the data to send.
 *  @param message_data Pointer to the data to send.
 *  @return True if the transaction is successfully initiated.
*/
bool lib_aci_echo_msg(uint8_t message_size, uint8_t *message_data);

/** @brief Sends an DTM command
 *  @details This function sends an @c DTM command to the radio. lib_aci
 *  calls the @ref lib_aci_transaction_finished_hook() function when the 
 *  @c CommandResponseEvent is received.
 *  @param dtm_command_msbyte Most significant byte of the DTM command.
 *  @param dtm_command_lsbyte Least significant byte of the DTM command.
 *  @return True if the transaction is successfully initiated.
*/
bool lib_aci_dtm_command(uint8_t dtm_command_msbyte, uint8_t dtm_command_lsbyte);
//@}

/** @} */

#endif /* LIB_ACI_H__ */
