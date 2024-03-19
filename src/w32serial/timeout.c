/* See LICENSE file for copyright and license details. */
#include "w32.h"

int
win32_serial_set_timeouts(MsgW32SerialDev *d, uint32_t rtimeout, uint32_t wtimeout)
{
	COMMTIMEOUTS timeouts = {
		.ReadIntervalTimeout         = 0,
		.ReadTotalTimeoutMultiplier  = 0,
		.ReadTotalTimeoutConstant    = rtimeout,
		.WriteTotalTimeoutMultiplier = 0,
		.WriteTotalTimeoutConstant   = wtimeout,
	};
	int err = d->h == INVALID_HANDLE_VALUE;
	err = !err && !SetCommTimeouts(d->h, &timeouts);
	return err? -1 : 0;
}
