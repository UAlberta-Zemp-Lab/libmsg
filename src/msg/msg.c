/* See LICENSE file for copyright and license details. */
#include <msg/msg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const uint8_t STX = 0x02;
static const uint8_t ETX = 0x03;

/*
 * allocates space for Msg data
 * returns true on success
 */
bool
msg_alloc(Msg *m)
{
	if (m->length == 0)
		return true;
	if ((m->data = malloc(m->length)) == NULL)
		return false;
	memset(m->data, 0, m->length);
	return true;
}

/* free data associated with Msg m */
void
msg_free(Msg *m)
{
	free(m->data);
	m->data = NULL;
}

uint8_t 
calculateChecksum(const uint8_t* data, uint16_t len) 
{
  uint8_t checksum = 0;
  for (uint16_t i = 0; i < len; ++i) 
  {
    checksum ^= data[i];
  }
  return checksum;
}

/**
 * Prints out the message data byte by byte as hex
 */
void
printMessage(const Msg *message)
{
	printf("Message bytes:\n");

	uint8_t *msgData = message->data;
	for (int i = 0; i < message->length; i++) {
		printf("%d: 0x%X\n", i, *msgData);
		msgData++;
	}
}

bool
buildMessage(Msg *message, uint16_t payloadSize, uint8_t *payload)
{
	uint8_t length = payloadSize + 7;
	message->length = length;

	if (!msg_alloc(message)) {
		return false;
	}

	uint8_t *data = (uint8_t *)message->data;

	*data = STX;
	data++;

	*data = ( length >> 8 ) & 0xff;
	data++;
	*data = length & 0xff;
	data++;

	*data = ( message->type >> 8 ) & 0xff;
	data++;
	*data = message->type & 0xff;
	data++;

	*data = calculateChecksum(message->data, length - 2);
	data++;

	*data = ETX;

	payload++;

	printMessage(message);


	return true;
}