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
	int p1[2], p2[2];

	if (pipe(p1) == -1)
		die("pipe(parent): failed");
	if (pipe(p2) == -1)
		die("pipe(child): failed");

	pid_t pid = fork();
	if (pid == -1) {
		die("fork(): failed");
	} else if (pid == 0) { /* child process */
		/* could close() some fds but the child will exit anyways */
		int child[2] = { p1[0], p2[1] };
		if (!msg_write(child, txm))
			die("child failed to write msg");
		exit(0);
	} else { /* parent process */
		int parent[2] = { p2[0], p1[1] };
		if (!msg_read(parent, rxm))
			die("parent failed to read msg");
	}

	close(p1[0]);
	close(p1[1]);
	close(p2[0]);
	close(p2[1]);
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
