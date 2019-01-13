#include <Timer.h>

static void timerHandler(int sig, siginfo_t *si, void *uc)
{	
	Timer* timer;
	timer = (Timer*) si->si_value.sival_ptr;

	timer->TimesUp();
}

int Timer::Start(char const *name, std::function<void()> userFunc, int expireMS, int intervalMS)
{
	sigevent te;
	itimerspec its;
	struct sigaction sa;
	int sigNo = SIGRTMIN + 1;

	/* Set up signal handler. */
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = timerHandler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(sigNo, &sa, NULL) == -1)
	{
		fprintf(stderr, "%s: Failed to setup signal handling for %s.\n", "Timer.cpp", name);
		return(-1);
	}

	/* Set and enable alarm */
	te.sigev_notify = SIGEV_SIGNAL;
	te.sigev_signo = sigNo;
	te.sigev_value.sival_ptr = this;
	timer_create(CLOCK_REALTIME,
		&te,
		&_timerID);

	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = intervalMS * 1000000;
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = expireMS * 1000000;
	timer_settime(_timerID,
		0,
		&its,
		NULL);

	_userFunc = userFunc;
	return(0);
}

void Timer::TimesUp()
{
	if (_userFunc != NULL)
	{
		_userFunc();
	}
}