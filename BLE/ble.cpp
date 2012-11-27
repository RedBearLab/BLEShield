/*

Copyright (c) 2012 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "hal_platform.h"
#include "lib_aci.h"
//#include "lib_traces.h"
#include "lib_mem.h"
#include "app.h"
#include "system.h"
#include "services.h"

#include "ble.h"

uint8_t write_ready = 1;
static uint8_t read_buf[65];
static uint8_t write_buf[65];
static uint8_t read_len = 0;
static uint8_t write_len = 0;
static uint8_t write_request = 0;


uint16_t ble_read_lib_ver(void)
{
    return LIBVER;
}

static unsigned char is_connected = 0;

void ble_begin()
{
	hal_aci_tl_io_config();  

	is_connected = 0;

	app_state = APP_INIT;
	lib_aci_init();

	ENABLE_INTERRUPTS();

	lib_aci_radio_reset();

	app_state = APP_SLEEP;
	on_process_app();
}

void ble_write(unsigned char data)
{
	write_buf[write_len] = data;
	write_len++;
}


void data_recv(unsigned char *data, unsigned char len)
{
	lib_mem_copy(&read_buf[read_len], data, len);
	read_len+=len;
}

unsigned char ble_available()
{
	return read_len;
}

int ble_read()
{
	if (read_len == 0)
		return -1;

	int data = read_buf[0];

	for (int i = 0; i < read_len-1; i++)
		read_buf[i] = read_buf[i+1];

	read_len--;

	return data;
}


void bleDidConnect()
{
    is_connected = 1;
}

void bleDidDisconnect()
{
    is_connected = 0;
}

unsigned char ble_connected()
{
    hal_aci_tl_poll_rdy_line();
    return is_connected;
}

void ble_do_events()
{
    if (!is_connected)
        write_ready = 1;

label1:
    hal_aci_tl_poll_rdy_line();

    if (write_len)
        if (write_ready)
	{	
			write_ready = 0;

			if (write_len <= 19)
			{
				if (is_connected)
					services_send_bleshield_rx_data((uint8_t *)write_buf, write_len, 1);
				write_len = 0;

				while (is_connected && !write_ready)
					hal_aci_tl_poll_rdy_line();
			}
			else
			{
				if (is_connected)
					services_send_bleshield_rx_data((uint8_t *)write_buf, 20, 1);

				if ((write_len - 20) == 0)
				{
					write_len = 0;
					hal_aci_tl_poll_rdy_line();
					return;
				}
				
				while (is_connected && !write_ready)
					hal_aci_tl_poll_rdy_line();

				if ((write_len - 20) > 0)
				{
					for (int i = 20; i < write_len; i++)
						write_buf[i-20] = write_buf[i];

					write_len -= 20;
				}

				goto label1;
			}
        }
}
