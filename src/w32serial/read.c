/* See LICENSE file for copyright and license details. */
#include "w32.h"

static ptrdiff_t
win32_serial_read(MsgW32SerialDev *d, uint8_t *b, ptrdiff_t blen)
{
	unsigned long read  = 0;
	unsigned long ublen = (unsigned long)blen;
	int err = blen < 0;
	err = !err && !ReadFile(d->h, b, ublen, &read, 0);
	return err? -1 : (ptrdiff_t)read;
}

MSG_READ_FN(w32read)
{
	ptrdiff_t r = win32_serial_read(ms->dev, data, count);
	return r > 0 && (size_t)r == count;
}
