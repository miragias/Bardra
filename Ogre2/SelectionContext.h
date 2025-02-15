#pragma once
#include "stdafx.h"
#include "Events.h"

typedef std::vector<Ogre::SceneNode*> AllowedSelectables;

class SelectionContext 
{
public:
    SelectionContext()
    {
        g_OnNodeDeleted.Subscribe([this](auto node){RemoveFromContext(node);});
    }

    void AddToContext(Ogre::SceneNode* node);
    void ClearContext();
    void RemoveFromContext(Ogre::SceneNode* node);
    AllowedSelectables* GetCurrentContext();
private:
    AllowedSelectables m_CurrentlyAllowedSelectables;
};
