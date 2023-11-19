#include <msg/msg.h>
#include <msg/unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

void
msg_pipe(Msg *txm, Msg *rxm)
{
	int parent[2], child[2];

	if (pipe(parent) == -1)
		die("pipe(parent): failed");
	if (pipe(child) == -1)
		die("pipe(child): failed");

	pid_t pid = fork();
	if (pid == -1) {
		die("fork(): failed");
	} else if (pid == 0) { /* child process */
		/* could close() some fds but the child will exit anyways */
		MsgUnistdDev d = { .rfd = parent[0], .wfd = child[1] };
		MsgHandle *h = msg_unistd_alloc(&d);
		if (!msg_write(h, txm))
			die("child failed to write msg");
		exit(0);
	} else { /* parent process */
		MsgUnistdDev d = { .rfd = child[0], .wfd = parent[1] };
		MsgHandle *h = msg_unistd_alloc(&d);
		if (!msg_read(h, rxm))
			die("parent failed to read msg");
		free(h);
	}

	close(parent[0]);
	close(parent[1]);
	close(child[0]);
	close(child[1]);
}

int
main(void)
{
	/* hack around meson's broken buffering */
	setvbuf(stdout, NULL, _IONBF, 0);

	uint8_t data[4] = { 0xde, 0xad, 0xbe, 0xef };
	Msg m1 = { .type = 0x1234, .length = 4, .data = data }, m2;

	msg_pipe(&m1, &m2);
	assert_msg_equal(&m1, &m2);

	return 0;
}
