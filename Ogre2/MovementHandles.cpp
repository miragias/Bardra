#include "stdafx.h"
#include "MovementHandles.h"

void MovementHandles::SetActive(bool active)
{
    m_IsActive = active;
    HandlesParent->setVisible(active);
}

bool MovementHandles::IsActive()
{
    return m_IsActive;
}

void MovementHandles::PlaceTo(Ogre::Vector3 targetPos)
{
    HandlesParent->setPosition(targetPos);
}

Axis MovementHandles::GetSelectedAxis(const Ogre::Vector2& mousePos, const Ogre::Camera* camera) const {

    Ogre::Ray mouseRay = camera->getCameraToViewportRay
    (
        mousePos.x,
        mousePos.y
    );
    std::cout << "mouse pos" << mousePos << "\n";

    float closest_distance = 1000000;
    Axis selected = None;

    // Check intersection with each handle
    std::pair<bool, float> xResult = mouseRay.intersects(m_XHandle->_getWorldAABB());
    if (xResult.first && xResult.second < closest_distance) {
        closest_distance = xResult.second;
        selected = X;
    }

    std::pair<bool, float> yResult = mouseRay.intersects(m_YHandle->_getWorldAABB());
    if (yResult.first && yResult.second < closest_distance) {
        closest_distance = yResult.second;
        selected = Y;
   }
   return selected;
}
