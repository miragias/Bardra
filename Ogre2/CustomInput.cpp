#include "CustomInput.h"

// CustomInput.cpp
#include "CustomInput.h"

bool CustomInput::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    std::cout << evt.keysym.sym << std::endl;
    if (evt.keysym.sym == 44)  // Check for key 'K'
    {
        std::cout << "Hello" << std::endl;
        return true;
    }
    return false;
}
