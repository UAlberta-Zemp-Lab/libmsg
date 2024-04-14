/* See LICENSE file for copyright and license details. */
#include <Stream.h>
#include <msg/arduino.h>
#include <msg/msgstream.h>
#include <stdbool.h>
#include <stdint.h>

static MSG_READ_FN(aread)
{
	Stream *d = (Stream *)ms->dev;
	return d->readBytes(data, count) == count;
}

static MSG_WRITE_FN(awrite)
{
	Stream *d = (Stream *)ms->dev;
	return d->write(data, count) == count;
}

static MSG_AVAILABLE_FN(available)
{
	Stream *d = (Stream *)ms->dev;
	return d->available() != 0;
}

void
msg_arduino_init(MsgStream *ms, Stream *s, uint8_t flags)
{
	msg_stream_init(ms, s, flags, 10, aread, awrite, available);
}
