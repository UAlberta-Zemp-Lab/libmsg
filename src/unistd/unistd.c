/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <msg/msg.h>
#include <msg/unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/*
 * read n bytes into buffer b
 * assumes b is at least n bytes long
 * returns whether all bytes were read
 */
static bool
read_bytes(int fd, void *b, size_t n)
{
	size_t r;
	while ((r = (size_t)read(fd, b, n)) == (size_t)-1 && errno == EINTR)
		;
	return r == n;
}

/*
 * reads everything in m except for the data
 * returns whether the operation succeeded
 */
static bool
read_hdr(int fd, Msg *m)
{
	return read_bytes(fd, &m->type, sizeof(m->type))
	       && read_bytes(fd, &m->length, sizeof(m->length));
}

/*
 * allocates and reads into Msg m->data
 * returns whether the operation succeeded
 */
static bool
read_data(int fd, Msg *m)
{
	if (!msg_alloc(m)
	    || (m->length > 0 && !read_bytes(fd, m->data, m->length))) {
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
write_bytes(int fd, const void *b, size_t n)
{
	size_t r;
	while ((r = (size_t)write(fd, b, n)) == (size_t)-1 && errno == EINTR)
		;
	return r == n;
}

/*
 * writes everything in m except for the data
 * doesn't check the response
 * returns whether operation succeeded
 */
static bool
write_hdr(int fd, Msg *m)
{
	return write_bytes(fd, &m->type, sizeof(m->type))
	       && write_bytes(fd, &m->length, sizeof(m->length));
}

/*
 * writes m's data
 * doesn't check the response
 * returns whether operation succeeded
 */
static bool
write_data(int fd, Msg *m)
{
	if (m->length == 0)
		return true;

	return write_bytes(fd, m->data, m->length);
}

/*
 * checks for a response from fd
 * returns Msg type of the response or MSG_ERR on error
 */
static uint16_t
check_response(int fd)
{
	Msg m;
	if (!read_hdr(fd, &m))
		return MSG_ERR;
	return m.type;
}

static bool
send_response(int fd, uint16_t mt)
{
	Msg m = { .type = mt, .length = 0 };
	return write_hdr(fd, &m);
}

/*
 * attempts to read a message using pipefd
 * pipefd conforms to pipe(2)
 * returns whether the operation succeeded
 */
bool
msg_read(int pipefd[2], Msg *m)
{
	for (;;) {
		/* try a read and retry if it fails */
		if (!read_hdr(pipefd[0], m)) {
			if (!send_response(pipefd[1], MSG_RETRY))
				return false;
			continue;
		}

		if (m->length > 0) {
			if (!send_response(pipefd[1], MSG_CONTINUE))
				return false;

			if (!read_data(pipefd[0], m)) {
				if (errno == ENOMEM) {
					send_response(pipefd[1], MSG_ERR);
					return false;
				}
				if (!send_response(pipefd[1], MSG_RETRY))
					return false;
				continue;
			}
		}

		return send_response(pipefd[1], MSG_ACK);
	}
}

/*
 * attempts to write a message using pipefd
 * pipefd conforms to pipe(2)
 * returns whether it succeeded
 */
bool
msg_write(int pipefd[2], Msg *m)
{
	for (;;) {
		if (!write_hdr(pipefd[1], m))
			return false;

		switch (check_response(pipefd[0])) {
		case MSG_RETRY:
			continue;
		case MSG_CONTINUE:
			break;
		case MSG_ACK:
			return true;
		case MSG_ERR:
		default:
			return false;
		}

		if (!write_data(pipefd[1], m))
			return false;

		switch (check_response(pipefd[0])) {
		case MSG_RETRY:
			continue;
		case MSG_ACK:
			return true;
		default:
			return false;
		}
	}
}
