#include <fcntl.h>
#include <msg/msg.h>
#include <msg/unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>

#include "../utils.h"

int
main(void)
{
	/* hack around meson's broken buffering */
	setvbuf(stdout, NULL, _IONBF, 0);

	uint8_t data[4] = { 0xde, 0xad, 0xbe, 0xef };
	Msg ms = { .type = 0xF00F, .length = 4, .data = data }, mr;
	int fd = open("/dev/ttyACM0", O_RDWR | O_SYNC), pipe[2] = { fd, fd };

	if (fd == -1)
		die("open(/dev/ttyACM0): failed");

	if (!msg_write(pipe, &ms))
		die("failed to write msg");

	if (!msg_read(pipe, &mr))
		die("failed to read msg");

	assert_msg_equal(&ms, &mr);
	dump_msg(&mr);

	return 0;
}
