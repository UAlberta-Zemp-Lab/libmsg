/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <msg/msg.h>
#include <msg/msgstream.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void
msg_stream_init(MsgStream *s, void *dev, uint8_t flags, uint32_t retries,
                bool (*available)(void *),
                bool (*write)(void *, void *, size_t),
                bool (*read)(void *, void *, size_t))
{
	s->dev = dev;
	s->flags = flags;
	s->retries = retries;
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

/*
 * attempts to read a message from stream s
 * returns whether the operation succeeded
 */
bool
msg_read(MsgStream *s, Msg *m)
{
	for (uint32_t retries = s->retries; retries; retries--) {
		/* try a read and retry if it fails */
		if (!read_hdr(s, m)) {
			if (!send_response(s, MSG_RETRY))
				return false;
			continue;
		}

		switch (m->type) {
		case MSG_ACK:
		case MSG_CONTINUE:
		case MSG_ERR:
		case MSG_RETRY:
			send_response(s, MSG_STOP);
			/* FALLTHROUGH */
		case MSG_STOP:
			return false;
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
	/* FIXME: should this send STOP ? */
	send_response(s, MSG_ERR);
	return false;
}

/*
 * attempts to write a message to stream
 * returns whether it succeeded
 */
bool
msg_write(MsgStream *s, Msg *m)
{
	for (uint32_t retries = s->retries; retries; retries--) {
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
			/* Possible simultaneous write */
			if (s->flags & MSG_MODE_MASTER)
				continue;
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
	return false;
}
