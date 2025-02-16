#include "stdafx.h"
#include "ClickObjectHandler.h"
#include "Common.h"
#include "MovementHandles.h"

void ClickObjectHandler::Cleanup()
{
    SelectionCommand.first = false;
    MoveCommandBuffer.clear();
}

SelectNodeCommand* ClickObjectHandler::GetSelectCommand()
{
    if(!SelectionCommand.first) return nullptr;
    return &SelectionCommand.second;
}

void ClickObjectHandler::GetFrameCommandsForInput(UserInput* input,Ogre::Camera* camera,
        AppContext& context, MovementHandles* movementHandles)
{
    if(input->LastMouseReleasedEvent.first)
    {
        std::cout << "Mouse released \n";
        context.CurrentlyClickingMovementAxis = None;
    }
    if(input->LastMouseClickedEvent.first)
    {
        std::cout << "Mouse clicked \n";
		Ogre::Vector2 mousePosition = Ogre::Vector2(input->LastMouseClickedEvent.second.x , input->LastMouseClickedEvent.second.y);
        TryClickOnSelectableObject(mousePosition, context.Selectables, camera);
        if(movementHandles->IsActive())
        {
            CheckClickOnHandle(mousePosition, context, movementHandles, camera);
        }
    }
    /*
    std::cout << "Input " << input->LastMouseMovedEvent << "\n";
    std::cout << "Dragging " << input->IsDragging() << "\n";
    std::cout << "Selected Node" << context.CurrentlySelectedNode << "\n";
    */
    if(input->LastMouseMovedEvent.first && input->IsDragging() && context.CurrentlySelectedNode)
    {
        std::cout << "Mouse moved while dragging \n";
        CheckUsedMoveHandles(camera, Ogre::Vector2(input->LastMouseMovedEvent.second.x , input->LastMouseMovedEvent.second.y), context);
    }
}

Ogre::Vector3 getMouseWorldPos(const Ogre::Vector2& mousePos, const Ogre::Plane& plane, const Ogre::Camera* camera) {
    Ogre::Ray mouseRay = camera->getCameraToViewportRay(mousePos.x, mousePos.y);
    std::pair<bool, float> result = mouseRay.intersects(plane);
    if (result.first) {
        return mouseRay.getPoint(result.second);
    }
    return Ogre::Vector3::ZERO;
}

void ClickObjectHandler::CheckClickOnHandle(Ogre::Vector2 mousePos,
                                            AppContext& context,
                                            MovementHandles* movementHandles,
                                            Ogre::Camera* camera)
{
    //Set the selectedAxis
    m_SelectedAxis = movementHandles->GetSelectedAxis(mousePos, camera);
    std::cout << "Set axis to: " << m_SelectedAxis << "\n";

    if (m_SelectedAxis != None) 
    {
        // Use the same plane calculation as mouseMove
        Ogre::Vector3 cameraDir = camera->getDerivedDirection();
        Ogre::Vector3 moveDir;

        switch (m_SelectedAxis) {
        case X: moveDir = Ogre::Vector3::UNIT_X; break;
        case Y: moveDir = Ogre::Vector3::UNIT_Y; break;
        }

        // Create the same plane as in mouseMove
        Ogre::Vector3 planeNormal = moveDir.crossProduct(cameraDir);
        planeNormal = planeNormal.crossProduct(moveDir);
        planeNormal.normalise();

        Ogre::Plane dragPlane(planeNormal, context.CurrentlySelectedNode->getPosition());
        //Set the last mouse position
        std::cout << "Set last position to: " << m_LastMousePos << "\n";
        m_LastMousePos = getMouseWorldPos(mousePos, dragPlane, camera);
    }
}

void ClickObjectHandler::CheckUsedMoveHandles(Ogre::Camera* camera, Ogre::Vector2 mousePosition,
                                              AppContext& AppContext) 
{
    // Create a plane perpendicular to the camera's view direction
    Ogre::Vector3 cameraDir = camera->getDerivedDirection();
    Ogre::Vector3 moveDir;

    switch (m_SelectedAxis) 
    {
        case X: moveDir = Ogre::Vector3::UNIT_X; break;
        case Y: moveDir = Ogre::Vector3::UNIT_Y; break;
        default: return;
    }

    // Create a plane that contains the movement axis and is as perpendicular 
    // to the camera view as possible
    Ogre::Vector3 planeNormal = moveDir.crossProduct(cameraDir);
    planeNormal = planeNormal.crossProduct(moveDir);
    planeNormal.normalise();

    auto targetNode = AppContext.CurrentlySelectedNode;
    Ogre::Plane dragPlane(planeNormal, targetNode->getPosition());
    Ogre::Vector3 currentPos = getMouseWorldPos(mousePosition, dragPlane, camera);

    if (currentPos != Ogre::Vector3::ZERO) 
    {
        std::cout << "CP: " << currentPos << "\n";
        std::cout << "lP: " << m_LastMousePos << "\n";
        Ogre::Vector3 delta = currentPos - m_LastMousePos;
        std::cout << "Delta: " << delta << "\n";
        float projection = delta.dotProduct(moveDir);
        Ogre::Vector3 movement = moveDir * projection;
        auto targetPos = targetNode->getPosition() + movement;
        MoveCommandBuffer.push_back({ AppContext.CurrentlySelectedNode, targetPos});
        MoveCommandBuffer.push_back({ AppContext.MoveHandlesNode, targetPos});

            /*
        auto it = m_VertexNodeToIndex->find(*m_TargetNode);
        if(it != m_VertexNodeToIndex->end())
        {
            int index = (*m_VertexNodeToIndex)[*m_TargetNode];
            (*m_Vertices)[index] = (*m_TargetNode)->getPosition();
        }
        */
        m_LastMousePos = currentPos;
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

    std::cout << selectableObjects.size();
    for(auto selectableNode : selectableObjects)
    {
        // Check if the ray intersects the object's AABB
        auto iteratingBoundingBox = selectableNode->_getWorldAABB();
        std::pair<bool, float> intersect = mouseRay.intersects(iteratingBoundingBox);
        if (intersect.first)
        {
            std::cout <<"Intersecting";
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
        std::cout <<"Happening";
        SelectionCommand.first = true;
        SelectionCommand.second = {closestNode};
        std::cout<< closestNode;
    }
}

