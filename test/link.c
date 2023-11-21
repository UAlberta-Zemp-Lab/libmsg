#include <msg/msg.h>

int
main(void)
{
	Msg m = { 0 };
	msg_alloc(&m);
	msg_free(&m);
	return 0;
}
