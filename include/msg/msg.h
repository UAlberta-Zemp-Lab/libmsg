/* See LICENSE file for copyright and license details. */
#ifndef _MSG_H
#define _MSG_H

#include <msg/platform.h>

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
LIBMSG_API bool msg_alloc(Msg *);
LIBMSG_API void msg_free(Msg *);

/**
 * Allocates memory in message and constructs the message in the following order
 *
 * - STX [1 Byte]
 * - Message Length [2 Bytes] // From STX -> ETX
 * - Message Type [2 Bytes]
 * - Payload [payloadLength Bytes]
 * - Checksum [1 Byte]
 * - ETX [1 Byte]
 *
 * Total length = 7 + payloadLength
 */
LIBMSG_API bool buildMessage(Msg *msg, uint16_t payloadLength, uint8_t *payload);

#ifdef __cplusplus
}
#endif

#endif /* _MSG_H */
