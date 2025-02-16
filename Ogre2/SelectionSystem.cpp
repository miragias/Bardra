#include "stdafx.h"

#include "SelectionSystem.h"
#include "Common.h"

void SelectionSystem::CheckUpdateSelectedNode(ClickObjectHandler* clickHandler, AppContext &context)
{
    auto setSelectionTo = clickHandler->GetSelectCommand();
    if(setSelectionTo)
    {
        std::cout <<"Set selection" << setSelectionTo << "\n";
        context.CurrentlySelectedNode = setSelectionTo->NodeSelected;
    }
}
