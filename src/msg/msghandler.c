/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <msg/msg.h>
#include <msg/msghandler.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct MsgHandle {
	void *dev;
	uint32_t timeout;
	uint32_t (*get_time)(void);
	bool (*available)(void *);
	bool (*read)(void *, void *, size_t);
	bool (*write)(void *, void *, size_t);
};

MsgHandle *
msg_handle_open(void *dev, uint32_t timeout, uint32_t (*get_time)(void),
                bool (*available)(void *),
                bool (*write)(void *, void *, size_t),
                bool (*read)(void *, void *, size_t))
{
	MsgHandle *h = malloc(sizeof(MsgHandle));
	h->dev = dev;
	h->timeout = timeout;
	h->get_time = get_time;
	h->available = available;
	h->write = write;
	h->read = read;
	return h;
}

void
msg_handle_close(MsgHandle *h)
{
	free(h);
}

bool
msg_available(MsgHandle *h)
{
	return h->available(h->dev);
}

/*
 * read n bytes into buffer b
 * assumes b is at least n bytes long
 * returns whether all bytes were read
 */
static bool
read_bytes(MsgHandle *h, void *b, size_t n)
{
	return h->read(h->dev, b, n);
}

/*
 * reads everything in m except for the data
 * returns whether the operation succeeded
 */
static bool
read_hdr(MsgHandle *h, Msg *m)
{
	return read_bytes(h, &m->type, sizeof(m->type))
	       && read_bytes(h, &m->length, sizeof(m->length));
}

/*
 * allocates and reads into Msg m->data
 * returns whether the operation succeeded
 */
static bool
read_data(MsgHandle *h, Msg *m)
{
	if (!msg_alloc(m)
	    || (m->length > 0 && !read_bytes(h, m->data, m->length))) {
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
write_bytes(MsgHandle *h, void *b, size_t n)
{
	return h->write(h->dev, b, n);
}

/*
 * writes everything in m except for the data
 * doesn't check the response
 * returns whether operation succeeded
 */
static bool
write_hdr(MsgHandle *h, Msg *m)
{
	return write_bytes(h, &m->type, sizeof(m->type))
	       && write_bytes(h, &m->length, sizeof(m->length));
}

/*
 * writes m's data
 * doesn't check the response
 * returns whether operation succeeded
 */
static bool
write_data(MsgHandle *h, Msg *m)
{
	if (m->length == 0)
		return true;

	return write_bytes(h, m->data, m->length);
}

/*
 * checks for a response from handle
 * returns Msg type of the response or MSG_ERR on error
 */
static uint16_t
check_response(MsgHandle *h)
{
	Msg m;
	if (!read_hdr(h, &m))
		return MSG_ERR;
	return m.type;
}

static bool
send_response(MsgHandle *h, uint16_t mt)
{
	Msg m = { .type = mt, .length = 0 };
	return write_hdr(h, &m);
}

static bool
timeout(MsgHandle *h, uint32_t start_time)
{
	uint32_t tstamp = h->get_time();
	return (tstamp < start_time) || (tstamp - start_time > h->timeout);
}

/*
 * attempts to read a message the handle h
 * returns whether the operation succeeded
 */
bool
msg_read(MsgHandle *h, Msg *m)
{
	uint32_t start_time = h->get_time();
	for (;;) {
		if (timeout(h, start_time)) {
			send_response(h, MSG_ERR);
			return false;
		}

		/* try a read and retry if it fails */
		if (!read_hdr(h, m)) {
			if (!send_response(h, MSG_RETRY))
				return false;
			continue;
		}

		switch (m->type) {
		case MSG_STOP:
			return true;
		case MSG_ACK:
		case MSG_CONTINUE:
		case MSG_RETRY:
			return send_response(h, MSG_STOP);
		}

		if (m->length > 0) {
			if (!send_response(h, MSG_CONTINUE))
				return false;

			if (!read_data(h, m)) {
				if (errno == ENOMEM) {
					send_response(h, MSG_ERR);
					return false;
				}
				if (!send_response(h, MSG_RETRY))
					return false;
				continue;
			}
		}

		return send_response(h, MSG_ACK);
	}
}

/*
 * attempts to write a message handle h
 * returns whether it succeeded
 */
bool
msg_write(MsgHandle *h, Msg *m)
{
	uint32_t start_time = h->get_time();
	for (;;) {
		if (timeout(h, start_time))
			return false;

		if (!write_hdr(h, m))
			return false;

		switch (check_response(h)) {
		case MSG_RETRY:
			continue;
		case MSG_CONTINUE:
			break;
		case MSG_ACK:
			return true;
		default:
			return false;
		}

		if (!write_data(h, m))
			return false;

		switch (check_response(h)) {
		case MSG_RETRY:
			continue;
		case MSG_ACK:
			return true;
		default:
			return false;
		}
	}
}
