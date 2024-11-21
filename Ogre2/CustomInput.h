#pragma once

#include <Bites/OgreApplicationContext.h>
#include <iostream> // For std::cout if not included elsewhere

// Forward declaration if needed
class CustomInput : public OgreBites::InputListener
{
	public:
		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
};