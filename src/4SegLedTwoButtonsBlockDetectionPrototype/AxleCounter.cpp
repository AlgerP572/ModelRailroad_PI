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
	if (_rightRailCount > 0 &&
		_leftRailCount > 0)
	{
		return;
	}

	_leftRailCount++;
	_leftOutputState = true;

	if (_rightRailCount > 0)
	{
		AxleCount++;

		_axelTimer.Start((char*)"LeftAxel",
			std::bind(&AxleCounter::ResetForNextAxel, this),
			100,
			0);
	}
}

void AxleCounter::RightRailISR()
{
	if (_rightRailCount > 0 &&
		_leftRailCount > 0)
	{
		return;
	}

	_rightRailCount++;
	_rightOutputState = true;

	if (_leftRailCount > 0)
	{		
		AxleCount--;

		_axelTimer.Start("RightAxel",
			std::bind(&AxleCounter::ResetForNextAxel, this),
			100,
			0);
	}
}

void AxleCounter::ResetForNextAxel()
{
	_leftRailCount = 0;
	_rightRailCount = 0;
	_leftOutputState = false;
	_rightOutputState = false;
}