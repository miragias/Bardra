#pragma once
#include "stdafx.h"

class UserInput : public OgreBites::InputListener
{
public:
    UserInput() {};
    bool IsDragging();

    void Cleanup();
    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) override;

    std::pair<bool, OgreBites::MouseMotionEvent> LastMouseMovedEvent;
    std::pair<bool, OgreBites::MouseButtonEvent> LastMouseClickedEvent;
    std::pair<bool, OgreBites::MouseButtonEvent> LastMouseReleasedEvent;
    std::pair<bool, OgreBites::MouseWheelEvent> LastMouseWheelEvent;
    std::pair<bool, OgreBites::KeyboardEvent> LastKeyboardEvent;
private:
    bool m_IsDragging = false;
};

