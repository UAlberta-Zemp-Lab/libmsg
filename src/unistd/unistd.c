/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <msg/unistd.h>
#include <poll.h>

MSG_WRITE_FN(uwrite)
{
	MsgUnistdDev *d = ms->dev;
	ptrdiff_t r;
	while ((r = write(d->wfd, data, count)) == -1 && errno == EINTR)
		;
	return r >= 0 && (size_t)r == count;
}

MSG_READ_FN(uread)
{
	MsgUnistdDev *d = ms->dev;
	ptrdiff_t r;
	while ((r = read(d->rfd, data, count)) == -1 && errno == EINTR)
		;
	return r >= 0 && (size_t)r == count;
}

MSG_AVAILABLE_FN(available)
{
	MsgUnistdDev *d = ms->dev;
	struct pollfd pfd = { .fd = d->rfd, .events = POLLIN };
	/* could have a variable timeout but its a bit clunky */
	int r = poll(&pfd, 1, 1);
	if (r > 0)
		if (pfd.revents & POLLIN)
			return true;
	return false;
}

void
msg_unistd_init(MsgStream *s, MsgUnistdDev *d, uint8_t flags)
{
	msg_stream_init(s, d, flags, 10, uread, uwrite, available);
}
