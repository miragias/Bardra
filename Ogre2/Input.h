#pragma once

#include "stdafx.h"
#include "Common.h"

struct InputEvent{
    EventType eventType;
    void* eventData;
};

std::vector<InputEvent> PollAllInput();
