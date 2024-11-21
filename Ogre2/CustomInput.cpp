#include "CustomInput.h"

// CustomInput.cpp
#include "CustomInput.h"

bool CustomInput::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    std::cout << evt.keysym.sym << std::endl;
    if (evt.keysym.sym == 27) //Escape key press close the app
    {
        m_Ctx->closeApp();
        return true;
    }
    return false;
}
