#include "device.h"

int
win32_serial_connect(MsgW32SerialDev *d, const char *port)
{
	/* FIXME: should security attributes allow child inheritance?
	 * It seems to work fine even though its disabled */
	d->h = CreateFileA(port, GENERIC_READ | GENERIC_WRITE, 0, NULL,
	                   OPEN_EXISTING, 0, NULL);
	if (d->h == INVALID_HANDLE_VALUE)
		return -1;

	DCB dcb;
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(d->h, &dcb))
		return -1;

	dcb.BaudRate = CBR_57600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	if (!SetCommState(d->h, &dcb))
		return -1;

	if (!GetCommState(d->h, &dcb))
		return -1;

	memcpy(&d->dcb, &dcb, sizeof(DCB));

	COMMTIMEOUTS timeouts = {
		.ReadIntervalTimeout = 1000,
		.ReadTotalTimeoutMultiplier = 1000,
		.ReadTotalTimeoutConstant = 1000,
		.WriteTotalTimeoutMultiplier = 1000,
		.WriteTotalTimeoutConstant = 1000,
	};

	if (!SetCommTimeouts(d->h, &timeouts))
		return -1;

	if (!PurgeComm(d->h, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT
	                         | PURGE_RXCLEAR))
		return -1;

	return 0;
}

void
win32_serial_disconnect(MsgW32SerialDev *d)
{
	CloseHandle(d->h);
}

ptrdiff_t
win32_serial_write(MsgW32SerialDev *d, uint8_t *b, ptrdiff_t len)
{
	unsigned long written = 0;
	if (len < 0)
		return -1;
	unsigned long ulen = (unsigned long)len;
	if (!WriteFile(d->h, b, ulen, &written, NULL))
		return -1;
	return (ptrdiff_t)written;
}

ptrdiff_t
win32_serial_read(MsgW32SerialDev *d, uint8_t *b, ptrdiff_t blen)
{
	unsigned long read = 0;
	if (blen < 0)
		return -1;
	unsigned long ublen = (unsigned long)blen;
	if (!ReadFile(d->h, b, ublen, &read, NULL))
		return -1;
	return (ptrdiff_t)read;
}

int
win32_serial_flush(MsgW32SerialDev *d)
{
	if (d->h == INVALID_HANDLE_VALUE)
		return -1;
	if (!PurgeComm(d->h, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT
	                         | PURGE_RXCLEAR))
		return -1;
	return 0;
}

ptrdiff_t
win32_serial_available(MsgW32SerialDev *d)
{
	if (d->h == INVALID_HANDLE_VALUE)
		return -1;

	DWORD errors;
	COMSTAT stat;

	if (!ClearCommError(d->h, &errors, &stat))
		return -1;

	return (ptrdiff_t)stat.cbInQue;
}

int
win32_serial_set_timeouts(MsgW32SerialDev *d, uint32_t rtimeout, uint32_t wtimeout)
{
	if (d->h == INVALID_HANDLE_VALUE)
		return -1;

	COMMTIMEOUTS timeouts = {
		.ReadIntervalTimeout = 0,
		.ReadTotalTimeoutMultiplier = 0,
		.ReadTotalTimeoutConstant = rtimeout,
		.WriteTotalTimeoutMultiplier = 0,
		.WriteTotalTimeoutConstant = wtimeout,
	};

	if (!SetCommTimeouts(d->h, &timeouts))
		return -1;
	return 0;
}
