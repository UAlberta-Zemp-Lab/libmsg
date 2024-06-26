/* See LICENSE file for copyright and license details. */
#include <msg/msg.h>
#include <msg/msgstream.h>
#include <stdbool.h>
#include <stdint.h>

void
msg_stream_init(MsgStream *s, void *dev, uint8_t flags, uint32_t retries,
                MsgReadFn *read, MsgWriteFn *write, MsgAvailableFn *available)
{
	s->read      = read;
	s->write     = write;
	s->available = available;
	s->dev       = dev;
	s->retries   = retries;
	s->flags     = flags;
}

/*
 * read n bytes into buffer b
 * assumes b is at least n bytes long
 * returns whether all bytes were read
 */
static bool
read_bytes(MsgStream *s, void *b, size_t n)
{
	return s->read(s, b, n);
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
static int
read_data(MsgStream *s, Msg *m)
{
	if (!msg_alloc(m))
		return -1;

	if (m->length > 0 && !read_bytes(s, m->data, m->length)) {
		msg_free(m);
		return 0;
	}
	return 1;
}

/*
 * writes n bytes into buffer b
 * assumes b is at least n bytes long
 * returns whether all bytes were written
 */
static bool
write_bytes(MsgStream *s, void *b, size_t n)
{
	return s->write(s, b, n);
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

			int r = read_data(s, m);
			if (r == -1) { /* failed allocation */
				send_response(s, MSG_ERR);
				return false;
			} else if (r == 0) { /* failed read */
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
