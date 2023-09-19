/* See LICENSE file for copyright and license details. */
#include <msg/msg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * allocates space for Msg data
 * returns true on success
 */
bool
msg_alloc(Msg *m)
{
	if (m->length == 0)
		return true;
	if ((m->data = malloc(m->length)) == NULL)
		return false;
	memset(m->data, 0, m->length);
	return true;
}

/* free data associated with Msg m */
void
msg_free(Msg *m)
{
	free(m->data);
	m->data = NULL;
}
