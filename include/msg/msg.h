/* See LICENSE file for copyright and license details. */
#ifndef _MSG_H
#define _MSG_H

#include <stdbool.h>
#include <stdint.h>

/* Reserved Message Types */
#define MSG_ERR      0xFF00
#define MSG_ACK      0xFF01
#define MSG_REJECT   0xFF02
#define MSG_CONTINUE 0xFF03
#define MSG_RETRY    0xFF04
#define MSG_DEBUG    0xFFFF

/* Type Definitions */
typedef struct {
	uint16_t type;
	uint16_t length;
	void *data;
} Msg;

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
bool msg_alloc(Msg *);
void msg_free(Msg *);

#ifdef __cplusplus
}
#endif

#endif /* _MSG_H */
