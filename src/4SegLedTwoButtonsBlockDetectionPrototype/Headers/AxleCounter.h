#pragma once

class AxleCounter
{
private:
	int _leftRailPin;
	int _rightRailPin;
	int _leftOutputPin;
	int _rightOutputPin;
	int _leftOutputState;
	int _rightOutputState;

	static const int maxAxleCounter = 1;
	static int _actualAxleCoutners;

	static AxleCounter* AxleCounters[maxAxleCounter];

	static void LeftRailIsr0();
	static void RightRailIsr0();

	static void(*isrFunctions[2 * maxAxleCounter])(void);

	void LeftRailISR();
	void RightRailISR();

public:
	int AxleCount;

	AxleCounter(int leftRailPin, int rightRailPin, int leftOutputPin = -1, int rightOutputPin = -1);
	void SysInit(void);
	void RefreshOutputStatus();
};
