#include <fcntl.h>
#include <msg/msg.h>
#include <msg/unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../utils.h"

int
main(void)
{
	/* hack around meson's broken buffering */
	setvbuf(stdout, NULL, _IONBF, 0);

	uint8_t data[4] = { 0xde, 0xad, 0xbe, 0xef };
	Msg ms = { .type = 0xF00F, .length = 4, .data = data }, mr;
	int fd = open("/dev/ttyACM0", O_RDWR | O_SYNC);
	MsgUnistdDev d = { fd, fd };
	MsgStream s;

	if (fd == -1)
		die("open(/dev/ttyACM0): failed");

	msg_unistd_init(&s, &d);

	/* flush any old data */
	lseek(fd, 0, SEEK_END);

	if (!msg_write(&s, &ms))
		die("failed to write msg");

	if (!msg_read(&s, &mr))
		die("failed to read msg");

	assert_msg_equal(&ms, &mr);
	dump_msg(&mr);

	return 0;
}
