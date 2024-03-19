#include <msg/w32serial.h>
#include <stddef.h>
#include <stdint.h>
#include <windows.h>

int win32_serial_connect(MsgW32SerialDev *, const char *);
void win32_serial_disconnect(MsgW32SerialDev *);
int win32_serial_flush(MsgW32SerialDev *);
int win32_serial_set_timeouts(MsgW32SerialDev *, uint32_t, uint32_t);

ptrdiff_t win32_serial_write(MsgW32SerialDev *, uint8_t *, ptrdiff_t);
ptrdiff_t win32_serial_read(MsgW32SerialDev *, uint8_t *, ptrdiff_t);
ptrdiff_t win32_serial_available(MsgW32SerialDev *);
