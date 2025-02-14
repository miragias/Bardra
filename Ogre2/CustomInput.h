#pragma once

#include "stdafx.h"
#include "MoveHandles.h"

class CustomInput : public OgreBites::InputListener
{
public:
    CustomInput(MoveHandles* moveHandles,
                Ogre::Camera* camera, Ogre::SceneNode* cameraNode,
                Ogre::SceneNode** currentlySelectedNode,
                Ogre::SceneManager* sceneManager,
                std::vector<Ogre::SceneNode*> world,
                Ogre::RenderWindow* renderWindow,
				std::function<void()> shutDownFunction)
    : m_MoveHandles(moveHandles)
    , m_CurrentlySelectedNode(currentlySelectedNode)
    , m_Camera(camera)
    , m_CameraNode(cameraNode)
    , m_LastMousePos(Ogre::Vector2::ZERO)
    , m_CameraDistance(100.0f)
    , m_IsCameraMoving(false)
    , m_IsRotating(false)
    , m_RotationCenter(Ogre::Vector3::ZERO)
    , m_SceneMgr(sceneManager)
    , m_World(world)
    , m_ShutDownAction(shutDownFunction)
    , m_RenderWindow(renderWindow)
    {
        m_CameraNode->setPosition(0, 0, m_CameraDistance);
        m_CameraNode->lookAt(m_RotationCenter, Ogre::Node::TS_WORLD);
    }


    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) override;

private:
    Ogre::RenderWindow* m_RenderWindow;
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
    std::function<void()> m_ShutDownAction;

    void checkAllNodesToChangeCurrentlySelected(Ogre::Vector2 mousePos);
    void focusNextObject();
    Ogre::Vector3 getPointOnGround(float screenX, float screenY);
};
