/* See LICENSE file for copyright and license details. */
#ifndef _MSGSTREAM_H
#define _MSGSTREAM_H

#include <stdbool.h>
#include <stddef.h>

/* generic message stream type */
typedef struct {
	void *dev;
	uint32_t timeout;
	uint32_t (*get_time)(void);
	bool (*available)(void *);
	bool (*read)(void *, void *, size_t);
	bool (*write)(void *, void *, size_t);
} MsgStream;

#ifdef __cplusplus
extern "C" {
#endif

bool msg_read(MsgStream *, Msg *);
bool msg_write(MsgStream *, Msg *);

void msg_stream_init(MsgStream *s, void *dev, uint32_t timeout,
                     uint32_t (*get_time)(void), bool (*available)(void *),
                     bool (*write)(void *, void *, size_t),
                     bool (*read)(void *, void *, size_t));

#ifdef __cplusplus
}
#endif

#endif /* _MSGSTREAM_H */
