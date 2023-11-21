/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <msg/msg.h>
#include <msg/msgstream.h>
#include <msg/unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

static bool
uwrite(MsgUnistdDev *d, void *b, size_t n)
{
	size_t r;
	while ((r = (size_t)write(d->wfd, b, n)) == (size_t)-1
	       && errno == EINTR)
		;
	return r == n;
}

static bool
uread(MsgUnistdDev *d, void *b, size_t n)
{
	size_t r;
	while ((r = (size_t)read(d->rfd, b, n)) == (size_t)-1 && errno == EINTR)
		;
	return r == n;
}

/* FIXME: this could be implemented */
static uint32_t
get_time(void)
{
	return 0;
}

static bool
available(MsgUnistdDev *d)
{
	struct pollfd pfd = { .fd = d->rfd, .events = POLLIN };
	/* could have a variable timeout but its a bit clunky */
	int r = poll(&pfd, 1, 1);
	if (r > 0)
		if (pfd.revents & POLLIN)
			return true;
	return false;
}

void
msg_unistd_init(MsgStream *s, MsgUnistdDev *d)
{
	msg_stream_init(s, d, 0, get_time, (bool (*)(void *))available,
	                (bool (*)(void *, void *, size_t))uwrite,
	                (bool (*)(void *, void *, size_t))uread);
}
