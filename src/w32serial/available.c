/* See LICENSE file for copyright and license details. */
#include "w32.h"

static ptrdiff_t
win32_serial_available(MsgW32SerialDev *d)
{
	COMSTAT stat = {0};
	int err = d->h == INVALID_HANDLE_VALUE;
	err = !err && !ClearCommError(d->h, 0, &stat);
	return err? -1 : (ptrdiff_t)stat.cbInQue;
}

MSG_AVAILABLE_FN(w32available)
{
	return win32_serial_available(ms->dev) > 0;
}
