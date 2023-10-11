#include <fcntl.h>
#include <msg/msg.h>
#include <msg/unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "../utils.h"

int
init_dev(const char *name)
{
	struct termios tio;
	int fd = open("/dev/ttyACM0", O_RDWR | O_SYNC | O_NOCTTY);
	if (fd == -1) {
		printf("open(%s): failed\n", name);
		exit(1);
	}

	cfmakeraw(&tio);
	if (tcsetattr(fd, TCSANOW, &tio) < 0) {
		fputs("tcsetattr: failed\n", stdout);
		exit(1);
	}

	/* flush any old data */
	tcflush(fd, TCIOFLUSH);

	return fd;
}

int
main(void)
{
	/* hack around meson's broken buffering */
	setvbuf(stdout, NULL, _IONBF, 0);

	uint8_t data[4] = { 0xde, 0xad, 0xbe, 0xef };
	Msg ms = { .type = 0xF00F, .length = 4, .data = data }, mr;

	int fd = init_dev("/dev/ttyACM0");
	MsgUnistdDev d = { fd, fd };
	MsgStream s;

	msg_unistd_init(&s, &d, 0);

	if (!msg_write(&s, &ms))
		die("failed to write msg");

	if (!msg_read(&s, &mr))
		die("failed to read msg");

	assert_msg_equal(&ms, &mr);
	dump_msg(&mr);

	return 0;
}
