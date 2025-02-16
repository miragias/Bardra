#include "stdafx.h"
#include "UserInput.h"
#include "Common.h"

bool UserInput::IsDragging()
{
    return m_IsDragging;
}

bool UserInput::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    LastMouseMovedEvent.second = evt;
    LastMouseMovedEvent.first = true;
    return true;
}

bool UserInput::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    m_IsDragging = true;
    LastMouseClickedEvent.second = evt;
    LastMouseClickedEvent.first = true;
    return true;
}

bool UserInput::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    m_IsDragging = false;
    LastMouseReleasedEvent.second = evt;
    LastMouseReleasedEvent.first = true;
    LastMouseMovedEvent.first = false;
    return true;
}

bool UserInput::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    LastMouseWheelEvent.second = evt;
    LastMouseWheelEvent.first = true;
    return true;
}

bool UserInput::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    LastKeyboardEvent.second = evt;
    LastKeyboardEvent.first = true;
    return true;
}

void UserInput::Cleanup()
{
    LastMouseClickedEvent.first = false;
    LastMouseReleasedEvent.first = false;
    LastMouseWheelEvent.first = false;
    LastKeyboardEvent.first = false;
}
