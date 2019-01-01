#include <Display.h>

#include <wiringPi.h>
#include <stdio.h>


// Contains the code to actual the pins for the four digit 7 segment display.
unsigned char const SegCode[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };

// the segcode to display for each digit.
unsigned char DatBuf[4] = { 0,0,0,0 };

FourDigitSevenSegmentDisplay::FourDigitSevenSegmentDisplay(int pin0, int pin1, int pin2, int pin3)
{
	_pin0 = pin0;
	_pin1 = pin1;
	_pin2 = pin2;
	_pin3 = pin3;
}

void FourDigitSevenSegmentDisplay::SysInit(void)
{
	pinMode(_pin0, OUTPUT);
	pinMode(_pin1, OUTPUT);
	pinMode(_pin2, OUTPUT);
	pinMode(_pin3, OUTPUT);

	digitalWrite(_pin0, HIGH);
	digitalWrite(_pin1, HIGH);
	digitalWrite(_pin2, HIGH);
	digitalWrite(_pin3, HIGH);

	// Digital Write byte uses PI IO pins 0-7 se we use those...
	for (int i = 0; i < 8; i++)
	{
		pinMode(i, OUTPUT);
		digitalWrite(i, HIGH);
	}
}

void FourDigitSevenSegmentDisplay::SetDisplayValue(int value)
{
	DatBuf[0] = SegCode[value % 10];
	DatBuf[1] = SegCode[value % 100 / 10];
	DatBuf[2] = SegCode[value % 1000 / 100];
	DatBuf[3] = SegCode[value / 1000];
}

void FourDigitSevenSegmentDisplay::Display(void)
{
	int i;

	for (i = 0; i < 100; i++) {
		digitalWrite(_pin0, 0);
		digitalWrite(_pin1, 1);
		digitalWrite(_pin2, 1);
		digitalWrite(_pin3, 1);
		digitalWriteByte(DatBuf[0]);

		delay(1);

		digitalWrite(_pin0, 1);
		digitalWrite(_pin1, 0);
		digitalWrite(_pin2, 1);
		digitalWrite(_pin3, 1);
		digitalWriteByte(DatBuf[1]);

		delay(1);

		digitalWrite(_pin0, 1);
		digitalWrite(_pin1, 1);
		digitalWrite(_pin2, 0);
		digitalWrite(_pin3, 1);
		digitalWriteByte(DatBuf[2]);

		delay(1);

		digitalWrite(_pin0, 1);
		digitalWrite(_pin1, 1);
		digitalWrite(_pin2, 1);
		digitalWrite(_pin3, 0);
		digitalWriteByte(DatBuf[3]);

		delay(1);
	}
}