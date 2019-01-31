#include <stdio.h>

#include <Display.h>
#include <Clock.h>
#include <Dma.h>
#include <Gpio.h>
#include <Pwm.h>
#include <AxleCounter.h>
#include <TrainBlockDetector.h>

// MAP of pins the program uses (PI pin numbers here)...

// The LED segments use digitalWriteByte so by definition use PI pins 0 - 7

// Display (These select the digit to display.)
#define   DisplayBit0    2
#define   DisplayBit1    3
#define   DisplayBit2    8
#define   DisplayBit3    7

#define   DisplayCharBit0 17
#define   DisplayCharBit1 18
#define   DisplayCharBit2 27
#define   DisplayCharBit3 22
#define   DisplayCharBit4 23
#define   DisplayCharBit5 24
#define   DisplayCharBit6 25
#define   DisplayCharBit7 4

static CharacterDisplayPins characterPins = CharacterDisplayPins(DisplayCharBit0,
	DisplayCharBit1,
	DisplayCharBit2,
	DisplayCharBit3,
	DisplayCharBit4,
	DisplayCharBit5,
	DisplayCharBit6,
	DisplayCharBit7);

// Output pulses for testing
//#define LeftRailOutput 21
//#define LeftRailOutput 22

// AxleCounter
#define LeftRailInput    16
#define RightRailInput   20

#define LeftRailOutput 19
#define RightRailOutput 26

static Dma dma("Dma Controller");
static Gpio gpio("Gpio Controller");
static Pwm pwm("Pwm Controller");
static Clock clockController("Clock Controller");

const int numPeripherals = 4;
static Peripheral** peripherals = new Peripheral*[numPeripherals];

static FourDigitSevenSegmentDisplay display(&gpio,
	DisplayBit0,
	DisplayBit1,
	DisplayBit2,
	DisplayBit3,
	&characterPins);

const int numAxleCounters = 1;
static AxleCounter axleCounters[numAxleCounters] =
{
	AxleCounter(&gpio,
		LeftRailInput,
		RightRailInput,
		LeftRailOutput,
		RightRailOutput)
};

const int numTrainBlockDtectors = 1;

static TrainBlockDetector trainBlockDetector[numTrainBlockDtectors] =
{
	TrainBlockDetector(&(axleCounters[0]))
};

void sysInit(void)
{
	peripherals[0] = &dma;
	peripherals[1] = &gpio;
	peripherals[2] = &pwm;
	peripherals[3] = &clockController;
	
	// Devices depend on peripherals so init them first.
	for (int i = 0; i < numPeripherals; i++)
	{
		peripherals[i]->SysInit();
	}

	// Now init devices... For now working definitions is
	// a device is made up of one or more dependent peripherls
	display.SysInit();
	for (int i = 0; i < numAxleCounters; i++)
	{
		axleCounters[i].SysInit();
	}
}

void sysUninit(void)
{
	for (int i = 0; i < numPeripherals; i++)
	{
		peripherals[i]->SysUninit();
	}
}

int main(void)
{

	// Now that library is intialized the individual devices
	// can be intialized.
	sysInit();

	gpio.TestPinExample(19);
	gpio.TestPinExample(26);

	clockController.TestExample();

	pwm.TestExample();

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
				

		dma.MemoryTestExample();
	}

	sysUninit();
	return 0;
}
