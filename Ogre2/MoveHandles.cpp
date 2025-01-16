#include "MoveHandles.h"
#include <iostream>

const float MoveHandles::HANDLE_SCALE = 0.002f;
const float MoveHandles::HANDLE_LENGTH = 0.4f;

void MoveHandles::setupHandles() {
    // Create main handle node
    mHandleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    // Create X axis handle (red)
    Ogre::Entity* xHandle = mSceneMgr->createEntity("handle_x", Ogre::SceneManager::PT_CUBE);
    mXHandle = mHandleNode->createChildSceneNode();
    mXHandle->attachObject(xHandle);
    mXHandle->setScale(HANDLE_LENGTH, HANDLE_SCALE, HANDLE_SCALE);
    mXHandle->setPosition(HANDLE_LENGTH / 2, 0, 0);
    xHandle->setMaterialName("Examples/Rockwall");

    // Create Y axis handle (green)
    Ogre::Entity* yHandle = mSceneMgr->createEntity("handle_y", Ogre::SceneManager::PT_CUBE);
    mYHandle = mHandleNode->createChildSceneNode();
    mYHandle->attachObject(yHandle);
    mYHandle->setScale(HANDLE_SCALE, HANDLE_LENGTH, HANDLE_SCALE);
    mYHandle->setPosition(0, HANDLE_LENGTH / 2, 0);
    yHandle->setMaterialName("Examples/Rockwall");

    // Create Z axis handle (blue)
    Ogre::Entity* zHandle = mSceneMgr->createEntity("handle_z", Ogre::SceneManager::PT_CUBE);
    mZHandle = mHandleNode->createChildSceneNode();
    mZHandle->attachObject(zHandle);
    mZHandle->setScale(HANDLE_SCALE, HANDLE_SCALE, HANDLE_LENGTH);
    mZHandle->setPosition(0, 0, HANDLE_LENGTH / 2);
    zHandle->setMaterialName("Examples/Rockwall");
}

void MoveHandles::update() {
    if (mTargetNode) {
        mHandleNode->setPosition(mTargetNode->getPosition());
    }
}

MoveHandles::Axis MoveHandles::getSelectedAxis(const Ogre::Vector2& mousePos) {
    Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
        mousePos.x,
        mousePos.y
    );

    float closest_distance = 1000000;
    Axis selected = None;

    // Check intersection with each handle
    std::pair<bool, float> xResult = mouseRay.intersects(mXHandle->_getWorldAABB());
    if (xResult.first && xResult.second < closest_distance) {
        closest_distance = xResult.second;
        selected = X;
    }

    std::pair<bool, float> yResult = mouseRay.intersects(mYHandle->_getWorldAABB());
    if (yResult.first && yResult.second < closest_distance) {
        closest_distance = yResult.second;
        selected = Y;
    }

    std::pair<bool, float> zResult = mouseRay.intersects(mZHandle->_getWorldAABB());
    if (zResult.first && zResult.second < closest_distance) {
        closest_distance = zResult.second;
        selected = Z;
    }

    return selected;
}

Ogre::Vector3 MoveHandles::getMouseWorldPos(const Ogre::Vector2& mousePos, const Ogre::Plane& plane) {
    Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.x, mousePos.y);
    std::pair<bool, float> result = mouseRay.intersects(plane);
    if (result.first) {
        return mouseRay.getPoint(result.second);
    }
    return Ogre::Vector3::ZERO;
}

bool MoveHandles::mousePressed(const Ogre::Vector2& mousePos) {
    std::cout << "hello";

    mSelectedAxis = getSelectedAxis(mousePos);
    std::cout << mSelectedAxis;
    if (mSelectedAxis != None) {
		std::cout << "set to dragging";
        mIsDragging = true;
        // Create a plane perpendicular to the selected axis for movement
        Ogre::Vector3 planeNormal;
        switch (mSelectedAxis) {
        case X: planeNormal = Ogre::Vector3::UNIT_X; break;
        case Y: planeNormal = Ogre::Vector3::UNIT_Y; break;
        case Z: planeNormal = Ogre::Vector3::UNIT_Z; break;
        default: return false;
        }
        Ogre::Plane dragPlane(planeNormal, mTargetNode->getPosition());
        mLastMousePos = getMouseWorldPos(mousePos, dragPlane);
        return true;
    }
    return false;
}

void MoveHandles::mouseMove(const Ogre::Vector2& mousePos) {
    if (!mIsDragging || mSelectedAxis == None) return;

    // Create a plane perpendicular to the camera's view direction
    Ogre::Vector3 cameraDir = mCamera->getDerivedDirection();
    Ogre::Vector3 moveDir;

    switch (mSelectedAxis) {
    case X: moveDir = Ogre::Vector3::UNIT_X; break;
    case Y: moveDir = Ogre::Vector3::UNIT_Y; break;
    case Z: moveDir = Ogre::Vector3::UNIT_Z; break;
    default: return;
    }

    // Create a plane that contains the movement axis and is as perpendicular 
    // to the camera view as possible
    Ogre::Vector3 planeNormal = moveDir.crossProduct(cameraDir);
    planeNormal = planeNormal.crossProduct(moveDir);
    planeNormal.normalise();

    Ogre::Plane dragPlane(planeNormal, mTargetNode->getPosition());
    Ogre::Vector3 currentPos = getMouseWorldPos(mousePos, dragPlane);

    if (currentPos != Ogre::Vector3::ZERO) {
        Ogre::Vector3 delta = currentPos - mLastMousePos;

        // Project the movement onto the selected axis
        float projection = delta.dotProduct(moveDir);
        Ogre::Vector3 movement = moveDir * projection;

        std::cout << "Delta: " << delta << " Movement: " << movement << std::endl;

        mTargetNode->translate(movement, Ogre::Node::TS_WORLD);

        /*
        // Also update the physics body if it exists
        if (mTargetNode->getName() == "bx") {  // Assuming this is your ball node
            Ogre::Vector3 newPos = mTargetNode->getPosition();
            btTransform trans;
            trans.setIdentity();
            trans.setOrigin(btVector3(newPos.x, newPos.y, newPos.z));
            // You'll need to store the rigid body pointer or get it from your physics world
            // boxRigidBody->setWorldTransform(trans);
        }
        */

        mLastMousePos = currentPos;
    }
}

bool MoveHandles::mouseReleased() {
    if (mIsDragging) {
		std::cout << "remove to dragging";
        mIsDragging = false;
        mSelectedAxis = None;
        return true;
    }
    return false;
}

void MoveHandles::setVisible(bool visible) {
    mHandleNode->setVisible(visible);
}