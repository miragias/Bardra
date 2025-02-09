#pragma once

#include <Bites/OgreApplicationContext.h>
#include <OgreCamera.h>
#include <OgreRay.h>
#include <OgreSceneQuery.h>
#include <iostream>
#include "MoveHandles.h"
#include "Ogre2.h"
#include <LinearMath/btVector3.h>
#include "EventSystem.h"

class CustomInput : public OgreBites::InputListener
{
public:
    CustomInput(CustomApplicationContext* ctx, Event<Ogre::SceneNode**> onSelectionChanged);

    void focusNextObject();
    Ogre::Vector3 getPointOnGround(float screenX, float screenY);
    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
    void checkAllNodesToChangeCurrentlySelected(Ogre::Vector2 mousePos);
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) override;

private:
    Ogre::RenderWindow* m_RenderWindow;
    OgreBites::ApplicationContext* m_Ctx;
    MoveHandles* m_MoveHandles;
    Ogre::Camera* m_Camera;
    Ogre::SceneNode* m_CameraNode;
    Ogre::SceneNode** m_CurrentlySelectedNode;
    Ogre::SceneManager* m_SceneMgr;
    Ogre::Vector2 m_LastMousePos;
    std::vector<Ogre::SceneNode*> m_World;
    float m_CameraDistance;
    bool m_IsCameraMoving;
    bool m_IsRotating;
    Ogre::Vector3 m_RotationCenter;
    bool m_WasRotatingLastFrame;
    size_t m_CurrentFocusIndex = 0;
    Event<Ogre::SceneNode**> m_OnSelectionChanged;
};
