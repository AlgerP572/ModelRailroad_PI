#include <AxleCounter.h>
#include <wiringPi.h>

int AxleCounter::_actualAxleCoutners = 0;

AxleCounter* AxleCounter::AxleCounters[AxleCounter::maxAxleCounter] =
{
	nullptr
};

void (*AxleCounter::isrFunctions[2 * AxleCounter::maxAxleCounter])(void) =
{
	AxleCounter::LeftRailIsr0,
	AxleCounter::RightRailIsr0,
};

void AxleCounter::LeftRailIsr0()
{
	AxleCounters[0]->LeftRailISR();
}

void AxleCounter::RightRailIsr0()
{
	AxleCounters[0]->RightRailISR();
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
		

	AxleCounters[_actualAxleCoutners] = this;
	wiringPiISR(_leftRailPin, INT_EDGE_FALLING, isrFunctions[2 * _actualAxleCoutners]);
	wiringPiISR(_rightRailPin, INT_EDGE_FALLING, isrFunctions[2 * _actualAxleCoutners + 1]);
	_actualAxleCoutners++;

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