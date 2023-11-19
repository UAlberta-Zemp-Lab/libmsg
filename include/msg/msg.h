/* See LICENSE file for copyright and license details. */
#ifndef _MSG_H
#define _MSG_H

#include <msg/msgtypes.h>
#include <stdbool.h>
#include <stdint.h>

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
