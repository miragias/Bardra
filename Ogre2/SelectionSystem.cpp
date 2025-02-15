#include "stdafx.h"

#include "SelectionSystem.h"
#include "Common.h"

void SelectionSystem::CheckUpdateSelectedNode(ClickObjectHandler* clickHandler, AppContext &context)
{
    auto setSelectionTo = clickHandler->GetSelectCommand();
    if(setSelectionTo)
    {
        context.CurrentlySelectedNode = setSelectionTo->NodeSelected;
    }
}
