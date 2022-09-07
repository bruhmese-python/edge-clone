#pragma once

#include <irrlicht.h>
#include "driverChoice.h"

using namespace irr;

class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	virtual bool KeysUp() const
	{
		return not (
			KeyIsDown[irr::KEY_KEY_W] or
			KeyIsDown[irr::KEY_KEY_A] or
			KeyIsDown[irr::KEY_KEY_S] or
			KeyIsDown[irr::KEY_KEY_D] or 
			KeyIsDown[irr::KEY_SPACE] 
			);
	}

	MyEventReceiver()
	{
		for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	bool KeyIsDown[KEY_KEY_CODES_COUNT];
private:
};