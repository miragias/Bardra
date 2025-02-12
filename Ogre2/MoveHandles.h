#pragma once
#include "stdafx.h"
#include "Common.h"
#include "Events.h"

class MoveHandles {
public:
    MoveHandles(Ogre::SceneManager* sceneMgr,
        Ogre::SceneNode** targetNode, Ogre::Camera* camera, btRigidBody* physicsBody,
        SelectionMode* currentSelectionmode)
        : mSceneMgr(sceneMgr)
        , mTargetNode(targetNode)
        , mCamera(camera)
        , mPhysicsBody(physicsBody)
        , mSelectedAxis(None)
        , mIsDragging(false)
    {
        setupHandles();
        OnSelectionModeChanged.Subscribe([this](SelectionMode newNode) {
            OnSelectionChanged(newNode);
		});
    }

    enum Axis {
        None,
        X,
        Y,
        Z
    };

    void update();
    bool mousePressed(const Ogre::Vector2& mousePos);
    bool mouseReleased();
    void mouseMove(const Ogre::Vector2& mousePos);
    void setVisible(bool visible);

private:
    static const float HANDLE_SCALE;
    static const float HANDLE_LENGTH;

    Axis mSelectedAxis;
    bool mIsDragging;
    btRigidBody* mPhysicsBody;
    Ogre::Vector3 mLastMousePos;
    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode** mTargetNode;
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mHandleNode;
    Ogre::SceneNode* mXHandle;
    Ogre::SceneNode* mYHandle;
    SelectionMode* m_CurrentSelectionMode;

    Event<SelectionMode> m_OnSelectionModeChanged;

    void setupHandles();
    void OnSelectionChanged(SelectionMode currentMode);
    Axis getSelectedAxis(const Ogre::Vector2& mousePos);
    Ogre::Vector3 getMouseWorldPos(const Ogre::Vector2& mousePos, const Ogre::Plane& plane);

};
