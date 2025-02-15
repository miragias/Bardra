#include "stdafx.h"
#include "ClickObjectHandler.h"
#include "Common.h"

void ClickObjectHandler::Cleanup()
{
    SelectionCommand.first = false;
    MoveCommand.first = false;
}

SelectNodeCommand* ClickObjectHandler::GetSelectCommand()
{
    if(!SelectionCommand.first) return nullptr;
    return &SelectionCommand.second;
}

MoveNodeCommand* ClickObjectHandler::GetMoveCommand()
{
    if(!MoveCommand.first) return nullptr;
    return &MoveCommand.second;
}

void ClickObjectHandler::GetFrameCommandsForInput(UserInput* input,
		const Selectables selectableObjects, Ogre::Camera* camera)
{
    if(input->LastMouseClickedEvent){
        TryClickOnSelectableObject(Ogre::Vector2(input->LastMouseClickedEvent->x , input->LastMouseClickedEvent->y), selectableObjects, camera);
    }
}

void ClickObjectHandler::TryClickOnSelectableObject(Ogre::Vector2 clickPosition, const Selectables selectableObjects, const Ogre::Camera* camera)
{
    Ogre::Ray mouseRay = camera->getCameraToViewportRay(
        clickPosition.x,
        clickPosition.y
    );

    float minDistance = std::numeric_limits<float>::infinity();
    Ogre::SceneNode* closestNode = nullptr;

    for(auto selectableNode : selectableObjects)
    {
        // Check if the ray intersects the object's AABB
        auto iteratingBoundingBox = selectableNode->_getWorldAABB();
        //TODO(JohnMir): Get this from the context
        //iteratingBoundingBox.scale(Ogre::Vector3(100,100,100));
        std::pair<bool, float> intersect = mouseRay.intersects(iteratingBoundingBox);
        if (intersect.first)
        {
            if (intersect.second < minDistance)
            {
                minDistance = intersect.second;
                closestNode = selectableNode;
            }
        }
    }

    // If a closest node was found, you can change the selection
    if (closestNode)
    {
        SelectionCommand.first = true;
        SelectionCommand.second = {closestNode};
        std::cout<< closestNode;
    }
}

