/* See LICENSE file for copyright and license details. */
#ifndef _MSG_UNISTD_H
#define _MSG_UNISTD_H

#include <msg/msg.h>
#include <msg/msghandler.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int wfd, rfd;
} MsgUnistdDev;

/* Prototypes */
MsgHandle *msg_unistd_alloc(MsgUnistdDev *d);

#ifdef __cplusplus
}
#endif

#endif /* _MSG_UNISTD_H */
