/* See LICENSE file for copyright and license details. */
#ifndef _MSG_UNISTD_H
#define _MSG_UNISTD_H

#include <msg/msg.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
bool msg_read(int[2], Msg *);
bool msg_write(int[2], Msg *);

#ifdef __cplusplus
}
#endif

#endif /* _MSG_UNISTD_H */
