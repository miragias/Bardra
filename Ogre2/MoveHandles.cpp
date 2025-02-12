#include "MoveHandles.h"
#include <iostream>
#include <LinearMath/btVector3.h>
#include <LinearMath/btDefaultMotionState.h>

const float MoveHandles::HANDLE_SCALE = 0.02f;
const float MoveHandles::HANDLE_LENGTH = 1.5f;

void MoveHandles::setupHandles() {

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
    mHandleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    // Create X axis handle (red)
    Ogre::Entity* xHandle = mSceneMgr->createEntity("handle_x", Ogre::SceneManager::PT_CUBE);
    mXHandle = mHandleNode->createChildSceneNode();
    mXHandle->attachObject(xHandle);
    mXHandle->setScale(HANDLE_LENGTH, HANDLE_SCALE, HANDLE_SCALE);
    mXHandle->setPosition(HANDLE_LENGTH / 2, 0, 0);
    xHandle->setMaterial(redMaterial);

    // Create Y axis handle (green)
    Ogre::Entity* yHandle = mSceneMgr->createEntity("handle_y", Ogre::SceneManager::PT_CUBE);
    mYHandle = mHandleNode->createChildSceneNode();
    mYHandle->attachObject(yHandle);
    mYHandle->setScale(HANDLE_SCALE, HANDLE_LENGTH, HANDLE_SCALE);
    mYHandle->setPosition(0, HANDLE_LENGTH / 2, 0);
    yHandle->setMaterial(greenMaterial);

    // Create Z axis handle (blue)
    Ogre::Entity* zHandle = mSceneMgr->createEntity("handle_z", Ogre::SceneManager::PT_CUBE);
    mZHandle = mHandleNode->createChildSceneNode();
    mZHandle->attachObject(zHandle);
    mZHandle->setScale(HANDLE_SCALE, HANDLE_SCALE, HANDLE_LENGTH);
    mZHandle->setPosition(0, 0, HANDLE_LENGTH / 2);
    zHandle->setMaterial(blueMaterial);
}

void MoveHandles::update() {
    if (mTargetNode) {
        mHandleNode->setPosition((*mTargetNode)->getPosition());
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
    mSelectedAxis = getSelectedAxis(mousePos);
    if (mSelectedAxis != None) {
        mIsDragging = true;

        // Use the same plane calculation as mouseMove
        Ogre::Vector3 cameraDir = mCamera->getDerivedDirection();
        Ogre::Vector3 moveDir;

        switch (mSelectedAxis) {
        case X: moveDir = Ogre::Vector3::UNIT_X; break;
        case Y: moveDir = Ogre::Vector3::UNIT_Y; break;
        case Z: moveDir = Ogre::Vector3::UNIT_Z; break;
        default: return false;
        }

        // Create the same plane as in mouseMove
        Ogre::Vector3 planeNormal = moveDir.crossProduct(cameraDir);
        planeNormal = planeNormal.crossProduct(moveDir);
        planeNormal.normalise();

        Ogre::Plane dragPlane(planeNormal, (*mTargetNode)->getPosition());
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

    Ogre::Plane dragPlane(planeNormal, (*mTargetNode)->getPosition());
    Ogre::Vector3 currentPos = getMouseWorldPos(mousePos, dragPlane);

    if (currentPos != Ogre::Vector3::ZERO) {
        Ogre::Vector3 delta = currentPos - mLastMousePos;
        float projection = delta.dotProduct(moveDir);
        Ogre::Vector3 movement = moveDir * projection;

        // Update physics body position
        if (mPhysicsBody) {
            btTransform trans;
            mPhysicsBody->getMotionState()->getWorldTransform(trans);
            btVector3 currentPhysicsPos = trans.getOrigin();
            btVector3 newPos = currentPhysicsPos + btVector3(movement.x, movement.y, movement.z);

            trans.setOrigin(newPos);
            mPhysicsBody->setWorldTransform(trans);

            // Reset velocity to prevent drift
            mPhysicsBody->setLinearVelocity(btVector3(0, 0, 0));
            mPhysicsBody->setAngularVelocity(btVector3(0, 0, 0));

            // Optionally, you might want to activate the body
            mPhysicsBody->activate(true);
        }
        else{
            (*mTargetNode)->setPosition((*mTargetNode)->getPosition() + movement);
        }
        mLastMousePos = currentPos;
    }
}

bool MoveHandles::mouseReleased() {
    if (mIsDragging) {
        mIsDragging = false;
        mSelectedAxis = None;
        return true;
    }
    return false;
}

void MoveHandles::setVisible(bool visible) {
    mHandleNode->setVisible(visible);
}
