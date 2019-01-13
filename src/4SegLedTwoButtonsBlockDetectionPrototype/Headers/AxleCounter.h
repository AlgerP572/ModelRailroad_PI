#pragma once

#include <signal.h>
#include <time.h>
#include <Timer.h>

class AxleCounter
{
private:
	int _leftRailPin;
	int _rightRailPin;
	int _leftOutputPin;
	int _rightOutputPin;
	int _leftOutputState;
	int _rightOutputState;

	int _leftRailCount;
	int _rightRailCount;

	Timer _axelTimer;

	static void LeftRailIsr0(void* arg);
	static void RightRailIsr0(void* arg);

	void LeftRailISR();
	void RightRailISR();
	void ResetForNextAxel();

public:
	int AxleCount;

	AxleCounter(int leftRailPin, int rightRailPin, int leftOutputPin = -1, int rightOutputPin = -1);
	void SysInit(void);
	void RefreshOutputStatus();
};
