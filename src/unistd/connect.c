/* See LICENSE file for copyright and license details. */
#include <msg/unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/* avoid useless define _DEFAULT_SOURCE */
static void
make_raw(struct termios *t)
{
	t->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	t->c_oflag &= ~(OPOST);
	t->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	t->c_cflag &= ~(CSIZE|PARENB);
	t->c_cflag |= CS8;
	t->c_cc[VMIN]  = 1;
	t->c_cc[VTIME] = 0;
}

bool
msg_unistd_serial_connect(MsgUnistdDev *d, const char *name)
{
	struct termios tio;
	d->wfd = open(name , O_RDWR | O_SYNC);
	d->rfd = d->wfd;

	if (d->wfd == -1)
		return false;

	make_raw(&tio);
	if (tcsetattr(d->wfd, TCSANOW, &tio) < 0) {
		close(d->wfd);
		return false;
	}

	/* flush any old data */
	tcflush(d->wfd, TCIOFLUSH);
	return true;
}
