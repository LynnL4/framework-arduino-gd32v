
/*
SS - D10
SCK - D11
MISO - D12
MOSI - D13
*/
#include <SPI.h>

const int slaveSelectPin = SS;

void setup()
{
	pinMode (slaveSelectPin, OUTPUT);
	SPI.begin();
}

void loop()
{
	digitalWrite(slaveSelectPin, LOW);
	SPI.transfer(0x55);
	digitalWrite(slaveSelectPin, HIGH);
	delay(10);
}
