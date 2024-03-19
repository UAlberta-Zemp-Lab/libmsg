/* See LICENSE file for copyright and license details. */
#include <msg/msgstream.h>
#include <msg/w32serial.h>
#include <stdbool.h>

#include "w32.h"

void
msg_w32serial_init(MsgStream *s, MsgW32SerialDev *d, uint8_t flags)
{
	msg_stream_init(s, d, flags, 10, w32read, w32write, w32available);
}
