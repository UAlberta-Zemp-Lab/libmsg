/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <msg/msg.h>
#include <msg/msghandler.h>
#include <msg/unistd.h>
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

MsgHandle *
msg_unistd_alloc(MsgUnistdDev *d)
{
	return msg_handle_alloc(d, 0, get_time,
	                        (bool (*)(void *, void *, size_t))uwrite,
	                        (bool (*)(void *, void *, size_t))uread);
}
