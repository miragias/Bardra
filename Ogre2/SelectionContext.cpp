#include "stdafx.h"
#include "SelectionContext.h"

void SelectionContext::AddToContext(Ogre::SceneNode* node)
{
    m_CurrentlyAllowedSelectables.push_back(node);
}

void SelectionContext::ClearContext()
{
    m_CurrentlyAllowedSelectables.clear();
}

void SelectionContext::RemoveFromContext(Ogre::SceneNode* node)
{
    auto it = std::remove(m_CurrentlyAllowedSelectables.begin(), m_CurrentlyAllowedSelectables.end(), node);
    m_CurrentlyAllowedSelectables.erase(it, m_CurrentlyAllowedSelectables.end());
}

AllowedSelectables* SelectionContext::GetCurrentContext()
{
    return &m_CurrentlyAllowedSelectables;
}

