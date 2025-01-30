#pragma once
#include <Ogre.h>
#include <btBulletDynamicsCommon.h>

class MoveHandles {
public:
    MoveHandles(Ogre::SceneManager* sceneMgr, Ogre::SceneNode** targetNode, Ogre::Camera* camera, btRigidBody* physicsBody)
        : mSceneMgr(sceneMgr)
        , mTargetNode(targetNode)
        , mCamera(camera)
        , mPhysicsBody(physicsBody)
        , mSelectedAxis(None)
        , mIsDragging(false)
    {
        setupHandles();
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
    void setupHandles();
    Axis getSelectedAxis(const Ogre::Vector2& mousePos);
    Ogre::Vector3 getMouseWorldPos(const Ogre::Vector2& mousePos, const Ogre::Plane& plane);

    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode** mTargetNode;
    Ogre::Camera* mCamera;

    Ogre::SceneNode* mHandleNode;
    Ogre::SceneNode* mXHandle;
    Ogre::SceneNode* mYHandle;
    Ogre::SceneNode* mZHandle;

    Axis mSelectedAxis;
    bool mIsDragging;
    btRigidBody* mPhysicsBody;
    Ogre::Vector3 mLastMousePos;
    static const float HANDLE_SCALE;
    static const float HANDLE_LENGTH;
};