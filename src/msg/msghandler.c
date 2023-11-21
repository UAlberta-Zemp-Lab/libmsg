/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <msg/msg.h>
#include <msg/msgstream.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void
msg_stream_init(MsgStream *s, void *dev, uint32_t timeout,
                uint32_t (*get_time)(void), bool (*available)(void *),
                bool (*write)(void *, void *, size_t),
                bool (*read)(void *, void *, size_t))
{
	s->dev = dev;
	s->timeout = timeout;
	s->get_time = get_time;
	s->available = available;
	s->write = write;
	s->read = read;
}

/*
 * read n bytes into buffer b
 * assumes b is at least n bytes long
 * returns whether all bytes were read
 */
static bool
read_bytes(MsgStream *s, void *b, size_t n)
{
	return s->read(s->dev, b, n);
}

/*
 * reads everything in m except for the data
 * returns whether the operation succeeded
 */
static bool
read_hdr(MsgStream *s, Msg *m)
{
	return read_bytes(s, &m->type, sizeof(m->type))
	       && read_bytes(s, &m->length, sizeof(m->length));
}

/*
 * allocates and reads into Msg m->data
 * returns whether the operation succeeded
 */
static bool
read_data(MsgStream *s, Msg *m)
{
	if (!msg_alloc(m)
	    || (m->length > 0 && !read_bytes(s, m->data, m->length))) {
		msg_free(m);
		return false;
	}
	return true;
}

/*
 * writes n bytes into buffer b
 * assumes b is at least n bytes long
 * returns whether all bytes were written
 */
static bool
write_bytes(MsgStream *s, void *b, size_t n)
{
	return s->write(s->dev, b, n);
}

/*
 * writes everything in m except for the data
 * doesn't check the response
 * returns whether operation succeeded
 */
static bool
write_hdr(MsgStream *s, Msg *m)
{
	return write_bytes(s, &m->type, sizeof(m->type))
	       && write_bytes(s, &m->length, sizeof(m->length));
}

/*
 * writes m's data
 * doesn't check the response
 * returns whether operation succeeded
 */
static bool
write_data(MsgStream *s, Msg *m)
{
	if (m->length == 0)
		return true;

	return write_bytes(s, m->data, m->length);
}

/*
 * checks for a response from stream
 * returns Msg type of the response or MSG_ERR on error
 */
static uint16_t
check_response(MsgStream *s)
{
	Msg m;
	if (!read_hdr(s, &m))
		return MSG_ERR;
	return m.type;
}

static bool
send_response(MsgStream *s, uint16_t mt)
{
	Msg m = { .type = mt, .length = 0 };
	return write_hdr(s, &m);
}

static bool
timeout(MsgStream *s, uint32_t start_time)
{
	uint32_t tstamp = s->get_time();
	return (tstamp < start_time) || (tstamp - start_time > s->timeout);
}

/*
 * attempts to read a message from stream s
 * returns whether the operation succeeded
 */
bool
msg_read(MsgStream *s, Msg *m)
{
	uint32_t start_time = s->get_time();
	for (;;) {
		if (timeout(s, start_time)) {
			send_response(s, MSG_ERR);
			return false;
		}

		/* try a read and retry if it fails */
		if (!read_hdr(s, m)) {
			if (!send_response(s, MSG_RETRY))
				return false;
			continue;
		}

		switch (m->type) {
		case MSG_STOP:
			return true;
		case MSG_ACK:
		case MSG_CONTINUE:
		case MSG_RETRY:
			return send_response(s, MSG_STOP);
		}

		if (m->length > 0) {
			if (!send_response(s, MSG_CONTINUE))
				return false;

			if (!read_data(s, m)) {
				if (errno == ENOMEM) {
					send_response(s, MSG_ERR);
					return false;
				}
				if (!send_response(s, MSG_RETRY))
					return false;
				continue;
			}
		}

		return send_response(s, MSG_ACK);
	}
}

/*
 * attempts to write a message to stream
 * returns whether it succeeded
 */
bool
msg_write(MsgStream *s, Msg *m)
{
	uint32_t start_time = s->get_time();
	for (;;) {
		if (timeout(s, start_time))
			return false;

		if (!write_hdr(s, m))
			return false;

		switch (check_response(s)) {
		case MSG_RETRY:
			continue;
		case MSG_CONTINUE:
			break;
		case MSG_ACK:
			return true;
		default:
			return false;
		}

		if (!write_data(s, m))
			return false;

		switch (check_response(s)) {
		case MSG_RETRY:
			continue;
		case MSG_ACK:
			return true;
		default:
			return false;
		}
	}
}
