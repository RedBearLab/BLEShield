
/*

Copyright (c) 2012 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef APP_H__
#define APP_H__

/** Application states */
typedef enum my_project_app_state_t {
  APP_INIT,               /**< Initialization */
  APP_SLEEP,              /**< Wake-up from Sleep state */
  APP_SETUP,              /**< Send configuration upload */
  APP_SETUP_FINISHED,     /**< Wait for event device_started in standby*/
  APP_STANDBY,            /**< Start advertising */
  APP_RUN,                /**< In Run state */
  APP_GOTO_SLEEP          /**< Go to Sleep*/
} my_project_app_state_t;

extern my_project_app_state_t app_state;

#define APP_RUN_ADV_PERIOD   600  /**< advertising period in multiple of 0.625 msec, must be between 0x0020 and 0x4000.*/
#define APP_ADVERTISING_TIME  300  /**< time during which the radio will advertise, in seconds */
#define APP_TIME_SLEEP        10  /**< time during which the device will stay in deep_sleep, in seconds */

/** @brief main procesing function.
 */
void on_process_app(void);

void radio_evt_error(void);

extern void bleDidConnect(void);
extern void bleDidDisconnect(void);
 
#endif // APP_H__

//@}
