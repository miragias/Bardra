#pragma once
#include "stdafx.h"
#include "Common.h"

class MovementHandles 
{
    const float HANDLE_SCALE = 0.01f;
    const float HANDLE_LENGTH = 1.5f;
public:
    MovementHandles(Ogre::SceneManager* sceneManager)
    {            
        Ogre::MaterialPtr baseMaterial = Ogre::MaterialManager::getSingleton().getByName("ColorMaterial");

        // Create Red Material
        Ogre::MaterialPtr redMaterial = baseMaterial->clone("RedMaterial");
        redMaterial->getTechnique(0)->getPass(0)->setEmissive(Ogre::ColourValue(1.0, 0.0, 0.0));

        // Create Green Material
        Ogre::MaterialPtr greenMaterial = baseMaterial->clone("GreenMaterial");
        greenMaterial->getTechnique(0)->getPass(0)->setEmissive(Ogre::ColourValue(0.0, 1.0, 0.0));

        // Create Blue Material
        Ogre::MaterialPtr blueMaterial = baseMaterial->clone("BlueMaterial");
        blueMaterial->getTechnique(0)->getPass(0)->setEmissive(Ogre::ColourValue(0.0, 0.0, 1.0));

        // Create main handle node
        HandlesParent = sceneManager->getRootSceneNode()->createChildSceneNode();

        // Create X axis handle (red)
        Ogre::Entity* xHandle = sceneManager->createEntity("handle_x", Ogre::SceneManager::PT_CUBE);
        m_XHandle = HandlesParent->createChildSceneNode();
        m_XHandle->attachObject(xHandle);
        m_XHandle->setScale(HANDLE_LENGTH, HANDLE_SCALE, HANDLE_SCALE);
        m_XHandle->setPosition(HANDLE_LENGTH / 2, 0, 0);
        xHandle->setMaterial(redMaterial);

        // Create Y axis handle (green)
        Ogre::Entity* yHandle = sceneManager->createEntity("handle_y", Ogre::SceneManager::PT_CUBE);
        m_YHandle = HandlesParent->createChildSceneNode();
        m_YHandle->attachObject(yHandle);
        m_YHandle->setScale(HANDLE_SCALE, HANDLE_LENGTH, HANDLE_SCALE);
        m_YHandle->setPosition(0, HANDLE_LENGTH / 2, 0);
        yHandle->setMaterial(greenMaterial);
    }
    void SetActive(bool visibility);
    void PlaceTo(Ogre::Vector3 targetPos);
    bool IsActive();
    Axis GetSelectedAxis(const Ogre::Vector2& mousePos, const Ogre::Camera* camera) const;
    Ogre::SceneNode* HandlesParent;
        
private:
    Ogre::SceneNode* m_XHandle;
    Ogre::SceneNode* m_YHandle;
    bool m_IsActive;
};
