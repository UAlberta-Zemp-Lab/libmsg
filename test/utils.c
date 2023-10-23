#include <msg/msg.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void
die_impl(const char *file, int line, const char *fmt, ...)
{
	va_list ap;
	printf("%s:%d: ", file, line);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
	abort();
}

void
assert_msg_equal_impl(const char *file, int line, Msg *m1, Msg *m2)
{
	if (m1->type != m2->type) {
		printf("%s:%d: type: 0x%04x != 0x%04x\n", file, line, m1->type,
		       m2->type);
		abort();
	}
	if (m1->length != m2->length) {
		printf("%s:%d: length: %d != %d\n", file, line, m1->length,
		       m2->length);
		abort();
	}
	if (m1->length > 0 && (m1->data == NULL || m2->data == NULL)) {
		printf("%s:%d: length = %d, m%d->data == NULL\n", file, line,
		       m1->length, m1->data == NULL ? 1 : 2);
		abort();
	}

	uint8_t *d1 = m1->data, *d2 = m2->data;
	for (size_t i = 0; i < m1->length; i++) {
		if (d1[i] != d2[i]) {
			printf("%s:%d: data[i = %zu]\n", file, line, i);
			abort();
		}
	}
}

void
dump_msg(Msg *m)
{
	printf("m: .type = 0x%04x; .length = 0x%04x; .data = 0x", m->type,
	       m->length);

	uint8_t *d = m->data;
	for (size_t i = 0; i < m->length; i++)
		printf("%02x ", d[i]);
	fputc('\n', stdout);
}
