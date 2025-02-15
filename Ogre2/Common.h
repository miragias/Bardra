#pragma once
#include "OgreInput.h"
#include "stdafx.h"

enum SelectionMode {
    OBJECT,VERTEX
};

enum EventType{
    MOUSE_CLICK, MOUSE_SCROLL, BUTTON_CLICK
};

struct SelectNodeCommand
{
    Ogre::SceneNode* NodeSelected;
};

struct MoveNodeCommand
{
    Ogre::SceneNode* NodeMoving;
    Ogre::Vector2 MoveBy;
};

