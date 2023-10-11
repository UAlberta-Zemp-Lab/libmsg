/* simple test that echos back recieved messages */
#include <Arduino.h>
#include <msg/arduino.h>
#include <msg/msg.h>

MsgStream s;

void
setup(void)
{
	Serial.begin(9600);
	msg_arduino_init(&s, &Serial, 0);
}

void
loop(void)
{
	Msg m = { 0 };
	if (s.available(s.dev) && msg_read(&s, &m))
		msg_write(&s, &m);
}
