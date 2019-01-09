#pragma once

class FourDigitSevenSegmentDisplay
{
private:
	int _pin0;
	int _pin1;
	int _pin2;
	int _pin3;
	
	unsigned char _datBuf[4] = { 0,0,0,0 };

	static unsigned char const SegCode[10];

public:
	FourDigitSevenSegmentDisplay(int pin0, int pin1, int pin2, int pin3);
	void SysInit(void);
	void SetDisplayValue(int value);
	void Display(void);
};