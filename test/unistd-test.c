#include <msg/msg.h>
#include <msg/unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

static void
msg_pipe(Msg *txm, Msg *rxm, bool expected_fail)
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
		MsgStream s;
		msg_unistd_init(&s, &d);
		if (!msg_write(&s, txm) && !expected_fail)
			die("child failed to write msg");
		exit(0);
	} else { /* parent process */
		MsgUnistdDev d = { .rfd = child[0], .wfd = parent[1] };
		MsgStream s;
		msg_unistd_init(&s, &d);
		if (!s.available(s.dev))
			die("parent has no data available");
		if (!msg_read(&s, rxm) && !expected_fail)
			die("parent failed to read msg");
	}

	close(parent[0]);
	close(parent[1]);
	close(child[0]);
	close(child[1]);
}

static void
test_tx_rx(void)
{
	uint8_t data[4] = { 0xde, 0xad, 0xbe, 0xef };
	Msg m1 = { .type = 0x1234, .length = 4, .data = data }, m2 = { 0 };

	msg_pipe(&m1, &m2, false);
	assert_msg_equal(&m1, &m2);
}

static void
test_msg_stop(void)
{
	Msg m1 = { .type = MSG_CONTINUE, .length = 0 }, m2 = { 0 };

	/* parent will read MSG_CONTINUE from child and send MSG_STOP
	 * child will fail after receiving MSG_STOP from parent
	 */
	msg_pipe(&m1, &m2, true);
	assert_msg_equal(&m1, &m2);
}

int
main(void)
{
	/* hack around meson's broken buffering */
	setvbuf(stdout, NULL, _IONBF, 0);

	test_tx_rx();
	test_msg_stop();

	return 0;
}
