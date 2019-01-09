#include <wiringPi.h>
#include <Display.h>
#include <AxleCounter.h>
#include <stdio.h>

// MAP of pins the program uses (PI pin numbers here)...

// Display (These select the digit to display.)
#define   DisplayBit0    8
#define   DisplayBit1    9
#define   DisplayBit2   10
#define   DisplayBit3   11
// The LED segments use digitalWriteByte so by definition use PI pins 0 - 7

// AxleCounter
#define LeftRailInput    27
#define RightRailInput    28

#define LeftRailOutput 24 // BCM pin 19 RPI 3B+
#define RightRailOutput 25 // BCM pin 26 RPI 3B+

static FourDigitSevenSegmentDisplay display(DisplayBit0, DisplayBit1, DisplayBit2, DisplayBit3);

const int numAxleCounters = 1;
static AxleCounter axleCounters[numAxleCounters] =
{
	AxleCounter(LeftRailInput, RightRailInput, LeftRailOutput, RightRailOutput)
};

void sysInit(void)
{
	display.SysInit();
	for (int i = 0; i < numAxleCounters; i++)
	{
		axleCounters[i].SysInit();
	}
}

int main(void)
{

	// Important: currently the interrupt extensions made for
	// this project ONLY work in WPI_MODE_PINS mode...
	if(wiringPiSetup() == -1)
	{

		// when initialize wiring failed,print message to screen
		printf("setup wiringPi failed !\n");
		return -1; 
	}

	// Now that library is intialized the individual devices
	// can be intialized.
	sysInit();

	while(1)
	{

		// Need to continuously update the dislpay
		// so it can be seen properly.
		display.SetDisplayValue(axleCounters[0].AxleCount);
		display.Display();

		// For prototype display the state of the inputs
		for (int i = 0; i < numAxleCounters; i++)
		{
			axleCounters[i].RefreshOutputStatus();
		}		
	}

	return 0;
}
