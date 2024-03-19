/* See LICENSE file for copyright and license details. */
#ifndef _MSG_W32SERIAL_H
#define _MSG_W32SERIAL_H

#include <msg/platform.h>
#include <msg/msgstream.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct {
	HANDLE h;
	DCB dcb;
} MsgW32SerialDev;

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
LIBMSG_API void msg_w32serial_init(MsgStream *, MsgW32SerialDev *, uint8_t flags);

/* Helper function for connecting to a COM port */
LIBMSG_API int  msg_w32serial_connect(MsgW32SerialDev *out, char *name);
LIBMSG_API void msg_w32serial_disconnect(MsgW32SerialDev *);

#ifdef __cplusplus
}
#endif

#endif /* _MSG_W32SERIAL_H */
