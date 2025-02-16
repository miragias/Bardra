#include "stdafx.h"
#include "CommandProcessor.h"
#include "Common.h"

void CommandProcessor::ProcessCommand(MoveNodeCommand moveNodeCommand)
{
    std::cout << "Process Move";
    moveNodeCommand.NodeMoving->setPosition(moveNodeCommand.MoveBy);
}

void CommandProcessor::ProcessMoveCommands(MoveCommandBuffer moveCommandBuffer)
{
    for(auto cmd : moveCommandBuffer)
    {
        ProcessCommand(cmd);
    }
}
