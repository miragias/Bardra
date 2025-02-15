#pragma once
#include "stdafx.h"

#include "Common.h"
#include "UserInput.h"

class ClickObjectHandler
{
    typedef std::vector<Ogre::SceneNode*> Selectables;

    public:
        void Cleanup();
        void GetFrameCommandsForInput(UserInput* input,
                const Selectables selectableObjects, Ogre::Camera* camera);
    private:
        std::pair<bool , SelectNodeCommand> SelectionCommand;
        std::pair<bool, MoveNodeCommand> MoveCommand;

        void TryClickOnSelectableObject(Ogre::Vector2 clickPosition, const Selectables selectableObjects, const Ogre::Camera* camera);
};


