#include <WiringPiExt.h>

#include <wiringPi.h>

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <poll.h>
#include <errno.h>
#include <string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <fcntl.h>

extern int wiringPiMode;
extern int *pinToGpio;
extern int *physToGpio;


static int sysFds[64] =
{
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
pthread_mutex_t pinMutex;
int pinPass;

static void(*isrFunctionsExt[64])(void*);

int waitForInterruptExt(int bcmPin, int mS)
{
	int fd, x;
	uint8_t c;
	struct pollfd polls;

	/**/ /*if (wiringPiMode == WPI_MODE_PINS)
		pin = pinToGpio[pin];
	else if (wiringPiMode == WPI_MODE_PHYS)
		pin = physToGpio[pin];*/

	if ((fd = sysFds[bcmPin]) == -1)
		return -2;

	// Setup poll structure

	polls.fd = fd;
	polls.events = POLLPRI | POLLERR;

	// Wait for it ...

	x = poll(&polls, 1, mS);

	// If no error, do a dummy read to clear the interrupt
	//	A one character read appars to be enough.

	if (x > 0)
	{
		lseek(fd, 0, SEEK_SET);	// Rewind
		(void)read(fd, &c, 1);	// Read & clear
	}

	return x;
}

void* interruptHandlerExt(UNU void *arg)
{
	int myPin;

	(void)piHiPri(55);	// Only effective if we run as root

	myPin = pinPass;
	pinPass = -1;

	for (;;)
	{
		int result = waitForInterruptExt(myPin, -1);
		if (result > 0)
			isrFunctionsExt[myPin](arg);
	}

	return NULL;
}

int wiringPiISRExt(int pin, int mode, void(*function)(void*), void* arg)
{
	pthread_t threadId;
	const char *modeS;
	char fName[64];
	char  pinS[8];
	pid_t pid;
	int   count, i;
	char  c;
	int   bcmGpioPin;

	if ((pin < 0) || (pin > 63))
		return wiringPiFailure(WPI_FATAL, "wiringPiISR: pin must be 0-63 (%d)\n", pin);

	///**/ if (wiringPiMode == WPI_MODE_UNINITIALISED)
	//	return wiringPiFailure(WPI_FATAL, "wiringPiISR: wiringPi has not been initialised. Unable to continue.\n");
	//else if (wiringPiMode == WPI_MODE_PINS)
	bcmGpioPin = wpiPinToGpio(pin);
	//else if (wiringPiMode == WPI_MODE_PHYS)
	//	bcmGpioPin = physToGpio[pin];
	//else
	//bcmGpioPin = pin;

	// Now export the pin and set the right edge
	//	We're going to use the gpio program to do this, so it assumes
	//	a full installation of wiringPi. It's a bit 'clunky', but it
	//	is a way that will work when we're running in "Sys" mode, as
	//	a non-root user. (without sudo)

	if (mode != INT_EDGE_SETUP)
	{
		/**/ if (mode == INT_EDGE_FALLING)
			modeS = "falling";
		else if (mode == INT_EDGE_RISING)
			modeS = "rising";
		else
			modeS = "both";

		sprintf(pinS, "%d", bcmGpioPin);

		if ((pid = fork()) < 0)	// Fail
			return wiringPiFailure(WPI_FATAL, "wiringPiISR: fork failed: %s\n", strerror(errno));

		if (pid == 0)	// Child, exec
		{
			/**/ if (access("/usr/local/bin/gpio", X_OK) == 0)
			{
				execl("/usr/local/bin/gpio", "gpio", "edge", pinS, modeS, (char *)NULL);
				return wiringPiFailure(WPI_FATAL, "wiringPiISR: execl failed: %s\n", strerror(errno));
			}
			else if (access("/usr/bin/gpio", X_OK) == 0)
			{
				execl("/usr/bin/gpio", "gpio", "edge", pinS, modeS, (char *)NULL);
				return wiringPiFailure(WPI_FATAL, "wiringPiISR: execl failed: %s\n", strerror(errno));
			}
			else
				return wiringPiFailure(WPI_FATAL, "wiringPiISR: Can't find gpio program\n");
		}
		else		// Parent, wait
			wait(NULL);

		// Now pre-open the /sys/class node - but it may already be open if
//	we are in Sys mode...

		if (sysFds[bcmGpioPin] == -1)
		{
			sprintf(fName, "/sys/class/gpio/gpio%d/value", bcmGpioPin);
			if ((sysFds[bcmGpioPin] = open(fName, O_RDWR)) < 0)
				return wiringPiFailure(WPI_FATAL, "wiringPiISR: unable to open %s: %s\n", fName, strerror(errno));
		}

		// Clear any initial pending interrupt

		ioctl(sysFds[bcmGpioPin], FIONREAD, &count);
		for (i = 0; i < count; ++i)
			read(sysFds[bcmGpioPin], &c, 1);

		isrFunctionsExt[bcmGpioPin] = function;

		pthread_mutex_lock(&pinMutex);
		pinPass = bcmGpioPin;
		pthread_create(&threadId, NULL, interruptHandlerExt, arg);
		while (pinPass != -1)
			delay(1);
		pthread_mutex_unlock(&pinMutex);
	}

		return 0;
}
