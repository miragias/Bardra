#pragma once

#include <Bites/OgreApplicationContext.h>
#include <iostream>

class CustomInput : public OgreBites::InputListener
{
	public:
        CustomInput(OgreBites::ApplicationContext* ctx) : m_Ctx(ctx){}

		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

    private:
        OgreBites::ApplicationContext* m_Ctx;
};
