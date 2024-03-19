/* See LICENSE file for copyright and license details. */
#include "w32.h"

static ptrdiff_t
win32_serial_write(MsgW32SerialDev *d, uint8_t *b, ptrdiff_t len)
{
	unsigned long written = 0;
	unsigned long ulen    = (unsigned long)len;
	int err = len < 0;
	err = !err && !WriteFile(d->h, b, ulen, &written, NULL);
	return err? -1 : (ptrdiff_t)written;
}

MSG_WRITE_FN(w32write)
{
	ptrdiff_t r = win32_serial_write(ms->dev, data, count);
	return r > 0 && (size_t)r == count;
}
