/* See LICENSE file for copyright and license details. */
#ifndef _MSG_ARDUINO_H
#define _MSG_ARDUINO_H

#include <msg/platform.h>

#include <Stream.h>
#include <msg/msg.h>
#include <msg/msgstream.h>
#include <stdint.h>

LIBMSG_API void msg_arduino_init(MsgStream *, Stream *);

#endif /* _MSG_ARDUINO_H */
