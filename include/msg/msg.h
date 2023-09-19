/* See LICENSE file for copyright and license details. */
#ifndef _MSG_H
#define _MSG_H

#include <stdbool.h>
#include <stdint.h>

/* Reserved Message Types */
#define MSG_ERR      0xF000
#define MSG_ACK      0xF001
#define MSG_REJECT   0xF002
#define MSG_CONTINUE 0xF003
#define MSG_RETRY    0xF004
#define MSG_SUCCESS  0xF005
#define MSG_FAILURE  0xF006
#define MSG_DEBUG    0xFFFF

/* Type Definitions */
typedef struct {
	uint16_t type;
	uint16_t length;
	void *data;
} Msg;

/* Prototypes */
bool msg_alloc(Msg *);
void msg_free(Msg *);

#endif /* _MSG_H */
