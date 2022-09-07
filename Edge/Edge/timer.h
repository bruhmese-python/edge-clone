#include <ITimer.h>
#include <time.h>

#pragma once
class timer : public irr::ITimer
{
private:
	time_t startt;
	irr::f32 interval;
	bool stopped = false;
public:
	irr::u32 getTime() const override;	//returns current time
	bool isStopped() const override;	//check whether stop flag is set
	void tick()override;				//update stop flag

	//Dummy overrides
	irr::u32 getRealTime() const override { return 0; }
	irr::ITimer::RealTimeDate getRealTimeAndDate() const override { return RealTimeDate(); }
	irr::f32 getSpeed() const override { return 0; }
	void setSpeed(irr::f32 speed = 1.0f) override {}
	void setTime(irr::u32 time) override {}
	void start() override {}
	void stop()override {}


	timer(irr::f32);
	~timer();
};

