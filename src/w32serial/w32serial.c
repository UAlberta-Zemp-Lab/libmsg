/* See LICENSE file for copyright and license details. */
#include <msg/msg.h>
#include <msg/msgstream.h>
#include <msg/w32serial.h>
#include <stdbool.h>

#include "device.h"

static bool
w32write(MsgW32SerialDev *d, void *b, size_t n)
{
	ptrdiff_t r = win32_serial_write(d, b, n);
	return r > 0 && (size_t)r == n;
}

static bool
w32read(MsgW32SerialDev *d, void *b, size_t n)
{
	ptrdiff_t r = win32_serial_read(d, b, n);
	return r > 0 && (size_t)r == n;
}

static bool
w32available(MsgW32SerialDev *d)
{
	return win32_serial_available(d) > 0;
}

int
msg_w32serial_connect(MsgW32SerialDev *d, char *name)
{
	return win32_serial_connect(d, name);
}

void
msg_w32serial_init(MsgStream *s, MsgW32SerialDev *d, uint8_t flags)
{
	msg_stream_init(s, d, flags, 10,
	                (bool (*)(void *))w32available,
	                (bool (*)(void *, void *, size_t))w32write,
	                (bool (*)(void *, void *, size_t))w32read);
}
