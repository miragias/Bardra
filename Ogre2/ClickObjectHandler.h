#pragma once
#include "stdafx.h"

#include "Common.h"
#include "UserInput.h"
#include "MovementHandles.h"

class ClickObjectHandler
{
typedef std::vector<Ogre::SceneNode*> Selectables;

public:
    void Cleanup();
    void GetFrameCommandsForInput(UserInput* input, Ogre::Camera* camera, AppContext& context,
                                  MovementHandles* moveHandles);
    SelectNodeCommand* GetSelectCommand();
    MoveCommandBuffer MoveCommandBuffer;

private:
    std::pair<bool , SelectNodeCommand> SelectionCommand;

    void TryClickOnSelectableObject(Ogre::Vector2 clickPosition, const Selectables selectableObjects, const Ogre::Camera* camera);
    void CheckClickOnHandle(Ogre::Vector2 mousePos, AppContext& context,
                                            MovementHandles* movementHandles,
                                            Ogre::Camera* camera);
    void CheckUsedMoveHandles(Ogre::Camera* camera, Ogre::Vector2 mousePosition,
                                                  AppContext& AppContext);
    Ogre::Vector3 m_LastMousePos;
    Axis m_SelectedAxis;
};


