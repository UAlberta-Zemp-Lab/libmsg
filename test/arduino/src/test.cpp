/* simple test that echos back recieved messages */
#include <Arduino.h>
#include <msg/arduino.h>
#include <msg/msg.h>

void
setup(void)
{
	Serial.begin(9600);
	msg_set_read_timeout(2000);
	msg_set_write_timeout(2000);
}

void
loop(void)
{
	Msg m = { 0 };
	if (Serial.available() && msg_read(&Serial, &m))
		msg_write(&Serial, &m);
}
