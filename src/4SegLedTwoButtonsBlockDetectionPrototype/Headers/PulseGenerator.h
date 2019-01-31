#pragma once

enum ActiveState {Low, High};

class PulseGenrator
{
private:
	int _pin;

public:
	ActiveState ActiveState = Low;

	PulseGenrator(int pin);
};
