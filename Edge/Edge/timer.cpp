#include "timer.h"


timer::timer(irr::f32 _interval)
{
	interval = _interval;
	time(&startt);
}

irr::u32 timer::getTime() const
{
	time_t endt;
	time(&endt);
	irr::f32 time_taken = double(endt - startt);
	return time_taken;
}
bool timer::isStopped() const
{
	return stopped;
}
void timer::tick()
{
	if (getTime() >= startt + interval)
		stopped = true;
}
timer::~timer() {}