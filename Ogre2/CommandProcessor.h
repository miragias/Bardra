#pragma once
#include "stdafx.h"
#include "Common.h"

class CommandProcessor
{
public:
    void ProcessMoveCommands(MoveCommandBuffer moveCommandBuffer);
private:
    void ProcessCommand(MoveNodeCommand moveNodeCommand);
};
