#include <msg/msg.h>

#define assert_msg_equal(m1, m2)                                               \
	assert_msg_equal_impl(__FILE__, __LINE__, (m1), (m2))

#define die(...) die_impl(__FILE__, __LINE__, __VA_ARGS__)

void assert_msg_equal_impl(const char *, int, Msg *, Msg *);
void die_impl(const char *, int, const char *, ...);
void dump_msg(Msg *);
