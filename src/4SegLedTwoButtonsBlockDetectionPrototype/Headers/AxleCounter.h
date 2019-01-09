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

	static void LeftRailIsr0(void* arg);
	static void RightRailIsr0(void* arg);

	void LeftRailISR();
	void RightRailISR();

public:
	int AxleCount;

	AxleCounter(int leftRailPin, int rightRailPin, int leftOutputPin = -1, int rightOutputPin = -1);
	void SysInit(void);
	void RefreshOutputStatus();
};
