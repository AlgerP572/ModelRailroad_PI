#pragma once

#include <signal.h>
#include <time.h>
#include <Timer.h>
#include <StopWatch.h>

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
	Stopwatch<std::chrono::microseconds> _axelTime;

	// TODO: configuration items...
	float _detectorLengthMm = 10.0;
	float _railroadScale = 22.5;
	float _axelScaleSpeedKmH;
	float _trainScaleSpeedKmH;

	static void LeftRailIsr0(void* arg);
	static void RightRailIsr0(void* arg);

	void LeftRailISR();
	void RightRailISR();
	void ResetForNextAxel();

	void CalculateSpeed();

public:
	int AxleCount;
	int TrainPresenceDetectionAxleCount;

	AxleCounter(int leftRailPin, int rightRailPin, int leftOutputPin = -1, int rightOutputPin = -1);
	void SysInit(void);
	void RefreshOutputStatus();
};
