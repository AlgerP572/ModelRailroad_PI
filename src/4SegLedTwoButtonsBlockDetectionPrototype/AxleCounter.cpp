#include <AxleCounter.h>
#include <wiringPi.h>
#include <WiringPiExt.h>

void AxleCounter::LeftRailIsr0(void* arg)
{
	AxleCounter* pCounter = (AxleCounter*)arg;
	pCounter->LeftRailISR();
}

void AxleCounter::RightRailIsr0(void* arg)
{
	AxleCounter* pCounter = (AxleCounter*)arg;
	pCounter->RightRailISR();
}

AxleCounter::AxleCounter(int leftRailPin, int rightRailPin, int leftOutputPin, int rightOutputPin)
{
	AxleCount = 0;
	_leftRailPin = leftRailPin;
	_rightRailPin = rightRailPin;

	// To not use output pins pass a -1. This lets you use the pins
	// for another purpose if desried.
	_leftOutputPin = leftOutputPin;
	_rightOutputPin = rightOutputPin;
}

void AxleCounter::SysInit()
{
	pinMode(_leftRailPin, INPUT);
	pinMode(_rightRailPin, INPUT);

	pullUpDnControl(_leftRailPin, PUD_UP);
	pullUpDnControl(_rightRailPin, PUD_UP);
	
	wiringPiISRExt(_leftRailPin,
		INT_EDGE_FALLING,
		LeftRailIsr0,
		this);
	wiringPiISRExt(_rightRailPin,
		INT_EDGE_FALLING,
		RightRailIsr0,
		this);

	if (_leftOutputPin > -1)
	{
		pinMode(_leftOutputPin, OUTPUT);
	}
	if (_rightOutputPin > -1)
	{
		pinMode(_rightOutputPin, OUTPUT);
	}
}

void AxleCounter::RefreshOutputStatus()
{
	digitalWrite(_leftOutputPin, _leftOutputState);
	digitalWrite(_rightOutputPin, _rightOutputState);
}

void AxleCounter::LeftRailISR()
{
	_leftOutputState = !_leftOutputState;

	// For now this is the wrong logic but used for early testing of LED display
	AxleCount++;
}

void AxleCounter::RightRailISR()
{
	_rightOutputState = !_rightOutputState;

	// For now this is the wrong logic but used for early testing of LED display
	AxleCount++;
}