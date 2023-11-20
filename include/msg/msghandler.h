/* See LICENSE file for copyright and license details. */
#ifndef _MSGHANDLER_H
#define _MSGHANDLER_H

#include <stdbool.h>
#include <stddef.h>

/* opaque message handle type */
typedef struct MsgHandle MsgHandle;

#ifdef __cplusplus
extern "C" {
#endif

bool msg_read(MsgHandle *, Msg *);
bool msg_write(MsgHandle *, Msg *);

/* FIXME: should this be hidden? */
MsgHandle *msg_handle_alloc(void *dev, uint32_t timeout,
                            uint32_t (*get_time)(void),
                            bool (*available)(void *),
                            bool (*write)(void *, void *, size_t),
                            bool (*read)(void *, void *, size_t));

bool msg_available(MsgHandle *);

#ifdef __cplusplus
}
#endif

#endif /* _MSGHANDLER_H */
