/* See LICENSE file for copyright and license details. */
#include "w32.h"

static int
win32_serial_connect(MsgW32SerialDev *d, const char *port)
{
	d->h = CreateFileA(port, GENERIC_READ | GENERIC_WRITE,
	                   0, 0, OPEN_EXISTING, 0, 0);
	if (d->h == INVALID_HANDLE_VALUE)
		return -1;

	DCB dcb;
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(d->h, &dcb))
		return -1;

	dcb.BaudRate = CBR_57600;
	dcb.ByteSize = 8;
	dcb.Parity   = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	if (!SetCommState(d->h, &dcb) || !GetCommState(d->h, &dcb))
		return -1;

	memcpy(&d->dcb, &dcb, sizeof(DCB));

	COMMTIMEOUTS timeouts = {
		.ReadIntervalTimeout         = 1000,
		.ReadTotalTimeoutMultiplier  = 1000,
		.ReadTotalTimeoutConstant    = 1000,
		.WriteTotalTimeoutMultiplier = 1000,
		.WriteTotalTimeoutConstant   = 1000,
	};

	int err = !SetCommTimeouts(d->h, &timeouts);
	err = !err && !PurgeComm(d->h, PURGE_TXABORT | PURGE_TXCLEAR |
	                               PURGE_RXABORT | PURGE_RXCLEAR);
	return err? -1 : 0;
}

int
msg_w32serial_connect(MsgW32SerialDev *d, char *name)
{
	return win32_serial_connect(d, name);
}

void
msg_w32serial_disconnect(MsgW32SerialDev *d)
{
	CloseHandle(d->h);
}
