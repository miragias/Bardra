#pragma once
#include "stdafx.h"

class UserInput : public OgreBites::InputListener
{
public:
    UserInput() {};

    void Cleanup();
    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) override;

    const OgreBites::MouseMotionEvent* LastMouseMovedEvent;
    const OgreBites::MouseButtonEvent* LastMouseClickedEvent;
    const OgreBites::MouseButtonEvent* LastMouseReleasedEvent;
    const OgreBites::MouseWheelEvent* LastMouseWheelEvent;
    const OgreBites::KeyboardEvent* LastKeyboardEvent;
};

