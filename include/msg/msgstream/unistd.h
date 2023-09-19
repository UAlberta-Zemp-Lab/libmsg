/* See LICENSE file for copyright and license details. */
#ifndef _MSGSTREAM_UNISTD_H
#define _MSGSTREAM_UNISTD_H

#include <msg/msg.h>
#include <stdbool.h>
#include <stdio.h>

/* Prototypes */
bool msg_stream_read_msg(int[2], Msg *);
bool msg_stream_write_msg(int[2], Msg *);

#endif /* _MSGSTREAM_UNISTD_H */
