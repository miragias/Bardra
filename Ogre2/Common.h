#pragma once
#include "OgreInput.h"
#include "stdafx.h"

struct MoveNodeCommand
{
    Ogre::SceneNode* NodeMoving;
    Ogre::Vector3 MoveBy;
};


typedef std::vector<Ogre::SceneNode*> Selectables;
typedef std::vector<MoveNodeCommand> MoveCommandBuffer;

enum SelectionMode {
    OBJECT,VERTEX
};

enum Axis {
    None,
    X,
    Y,
};


enum EventType{
    MOUSE_CLICK, MOUSE_SCROLL, BUTTON_CLICK
};

struct SelectNodeCommand
{
    Ogre::SceneNode* NodeSelected;
};

//When user lifts mouse from move handle
struct ReleaseMoveHandleCommand {};

struct AppContext{
    Ogre::SceneNode* CurrentlySelectedNode;
    Ogre::SceneNode* MoveHandlesNode;
    Selectables Selectables;
    Axis CurrentlyClickingMovementAxis;
};

