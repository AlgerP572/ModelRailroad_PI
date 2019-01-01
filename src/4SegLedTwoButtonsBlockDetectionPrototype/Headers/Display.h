#pragma once

class FourDigitSevenSegmentDisplay
{
private:
	int _pin0;
	int _pin1;
	int _pin2;
	int _pin3;

public:
	FourDigitSevenSegmentDisplay(int pin0, int pin1, int pin2, int pin3);
	void SysInit(void);
	void SetDisplayValue(int value);
	void Display(void);
};