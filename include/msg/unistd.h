/* See LICENSE file for copyright and license details. */
#ifndef _MSG_UNISTD_H
#define _MSG_UNISTD_H

#include <msg/platform.h>

#include <msg/msg.h>
#include <msg/msgstream.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

typedef struct { int wfd, rfd; } MsgUnistdDev;

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
LIBMSG_API void msg_unistd_init(MsgStream *, MsgUnistdDev *, uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif /* _MSG_UNISTD_H */
