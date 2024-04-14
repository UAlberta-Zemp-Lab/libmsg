/* See LICENSE file for copyright and license details. */
#ifndef _MSGSTREAM_H
#define _MSGSTREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MSG_FLAG_MODE 0

#define MSG_MODE_SLAVE  (0 << MSG_FLAG_MODE)
#define MSG_MODE_MASTER (1 << MSG_FLAG_MODE)

/* forward declare to use as pointer in MsgStream functions */
struct MsgStream;

#define MSG_AVAILABLE_FN(name) bool name(struct MsgStream *ms)
typedef MSG_AVAILABLE_FN(MsgAvailableFn);

#define MSG_READ_FN(name) bool name(struct MsgStream *ms, uint8_t *data, size_t count)
typedef MSG_READ_FN(MsgReadFn);

#define MSG_WRITE_FN(name) bool name(struct MsgStream *ms, uint8_t *data, size_t count)
typedef MSG_WRITE_FN(MsgWriteFn);

/* generic message stream type */
typedef struct MsgStream {
	MsgAvailableFn *available;
	MsgReadFn      *read;
	MsgWriteFn     *write;
	void           *dev;
	uint32_t       retries;
	uint8_t        flags;
} MsgStream;

#ifdef __cplusplus
extern "C" {
#endif

LIBMSG_API bool msg_read(MsgStream *, Msg *);
LIBMSG_API bool msg_write(MsgStream *, Msg *);

LIBMSG_API void msg_stream_init(MsgStream *s, void *dev, uint8_t flags,
                                uint32_t retries,
                                MsgReadFn *read, MsgWriteFn *write,
                                MsgAvailableFn *available);

#ifdef __cplusplus
}
#endif

#endif /* _MSGSTREAM_H */
