/* See LICENSE file for copyright and license details. */
#include <Stream.h>
#include <core_pins.h>
#include <msg/arduino.h>
#include <msg/msg.h>
#include <msg/msgstream.h>
#include <stdbool.h>
#include <stdint.h>

static bool
aread(Stream *s, uint8_t *b, size_t n)
{
	return s->readBytes(b, n) == n;
}

static bool
awrite(Stream *s, uint8_t *b, size_t n)
{
	return s->write(b, n) == n;
}

static bool
available(Stream *s)
{
	return s->available() != 0;
}

void
msg_arduino_init(MsgStream *ms, Stream *s, uint8_t flags)
{
	msg_stream_init(ms, s, flags, 10, (bool (*)(void *))available,
	                (bool (*)(void *, void *, size_t))awrite,
	                (bool (*)(void *, void *, size_t))aread);
}
