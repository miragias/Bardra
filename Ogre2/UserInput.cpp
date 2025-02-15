#include "stdafx.h"
#include "UserInput.h"
#include "Common.h"

bool UserInput::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    LastMouseMovedEvent = &evt;
    return true;
}

bool UserInput::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    LastMouseClickedEvent = &evt;
    return true;
}

bool UserInput::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    LastMouseReleasedEvent = &evt;
    return true;
}

bool UserInput::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    LastMouseWheelEvent = &evt;
    return true;
}

bool UserInput::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    LastKeyboardEvent = &evt;
    return true;
}

void UserInput::Cleanup()
{
    LastMouseMovedEvent = nullptr;;
    LastMouseClickedEvent = nullptr;;
    LastMouseReleasedEvent = nullptr;;
    LastMouseWheelEvent = nullptr;;
    LastKeyboardEvent = nullptr;;
}
