/* See LICENSE file for copyright and license details. */
#include "w32.h"

int
win32_serial_flush(MsgW32SerialDev *d)
{
	int err = d->h == INVALID_HANDLE_VALUE;
	err = !err && !PurgeComm(d->h, PURGE_TXABORT | PURGE_TXCLEAR |
	                               PURGE_RXABORT | PURGE_RXCLEAR);
	return err? -1 : 0;
}
