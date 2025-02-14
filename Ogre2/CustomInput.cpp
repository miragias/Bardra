#include "stdafx.h"
#include "CustomInput.h"

void CustomInput::focusNextObject()
{
    if (m_World.empty()) return;

    // Increment index and wrap around
    m_CurrentFocusIndex = (m_CurrentFocusIndex + 1) % m_World.size();

    // Get the next object to focus on
    Ogre::SceneNode* targetNode = m_World[m_CurrentFocusIndex];

    // Get the center position of the object
    Ogre::Vector3 objectCenter = targetNode->_getDerivedPosition();

    // Calculate the desired camera position
    // Move back by m_CameraDistance units along the current view direction
    Ogre::Vector3 viewDirection = m_CameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
    Ogre::Vector3 newCameraPosition = objectCenter - (viewDirection * m_CameraDistance);

    // Smoothly move the camera to the new position
    m_CameraNode->setPosition(newCameraPosition);

    // Make the camera look at the object
    m_CameraNode->lookAt(objectCenter, Ogre::Node::TS_WORLD);

    // Update the rotation center for orbital rotation
    m_RotationCenter = objectCenter;

    // Update the currently selected node
    *m_CurrentlySelectedNode = targetNode;
    g_OnSelectionChangedEvent.Invoke(&targetNode);
}

Ogre::Vector3 CustomInput::getPointOnGround(float screenX, float screenY)
{
    Ogre::RenderWindow* window = m_RenderWindow;

    // Convert screen coordinates to ray
    Ogre::Ray mouseRay = m_Camera->getCameraToViewportRay(
        screenX / float(window->getWidth()),
        screenY / float(window->getHeight())
    );

    // Create plane at y=0
    Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0);

    // Find intersection
    std::pair<bool, float> result = mouseRay.intersects(ground);
    if (result.first) {
        return mouseRay.getPoint(result.second);
    }

    // If no intersection, return a point at some distance along the ray
    return mouseRay.getPoint(100);
}

bool CustomInput::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if (evt.keysym.sym == 27) { // Escape key
        return true;
    }

    if (evt.keysym.sym == '\t') { // Tab key
        focusNextObject();
        return true;
    }
    return false;
}

bool CustomInput::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    Ogre::RenderWindow* window = m_RenderWindow;
    Ogre::Vector2 currentPos(
        evt.x / float(window->getWidth()),
        evt.y / float(window->getHeight())
    );

    if (m_CameraNode) {
        Ogre::Vector2 diff = currentPos - m_LastMousePos;

        if (m_IsRotating && evt.type & OgreBites::BUTTON_RIGHT) {
            // Yaw (left/right rotation) with horizontal mouse movement
            float yawAngle = -diff.x * 180.0f; // Adjust sensitivity as needed
            m_CameraNode->yaw(Ogre::Degree(yawAngle));

            // Pitch (up/down rotation) with vertical mouse movement
            float pitchAngle = -diff.y * 180.0f; // Adjust sensitivity as needed
            m_CameraNode->pitch(Ogre::Degree(pitchAngle));
        }

        // Pan camera with middle mouse button
        if (m_IsCameraMoving && evt.type & OgreBites::BUTTON_MIDDLE) {
            float dx = diff.x * m_CameraDistance * 2.0f;
            float dy = diff.y * m_CameraDistance * 2.0f;

            Ogre::Vector3 right = m_CameraNode->getOrientation() * Ogre::Vector3::UNIT_X;
            Ogre::Vector3 up = m_CameraNode->getOrientation() * Ogre::Vector3::UNIT_Y;

            m_CameraNode->translate(right * -dx + up * dy);
        }
    }

    if (m_MoveHandles) {
        m_MoveHandles->MouseMove(currentPos);
    }

    m_LastMousePos = currentPos;
    return true;
}

bool CustomInput::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    Ogre::RenderWindow* window = m_RenderWindow;
    m_LastMousePos = Ogre::Vector2(
        evt.x / float(window->getWidth()),
        evt.y / float(window->getHeight())
    );

    checkAllNodesToChangeCurrentlySelected(m_LastMousePos);
    if (evt.button == OgreBites::BUTTON_MIDDLE) {
        m_IsCameraMoving = true;
        return true;
    }
    else if (evt.button == OgreBites::BUTTON_RIGHT) {
        // Set rotation center to clicked point
        m_RotationCenter = getPointOnGround(evt.x, evt.y);
        m_IsRotating = true;
        return true;
    }

    if (m_MoveHandles && evt.button == OgreBites::BUTTON_LEFT) {
        return m_MoveHandles->MousePressed(m_LastMousePos);
    }
    return true;
}

void CustomInput::checkAllNodesToChangeCurrentlySelected(Ogre::Vector2 mousePos)
{
    Ogre::Ray mouseRay = m_Camera->getCameraToViewportRay(
        mousePos.x,
        mousePos.y
    );

    float minDistance = std::numeric_limits<float>::infinity();
    Ogre::SceneNode* closestNode = nullptr;

    for (size_t i = 0; i < m_World.size(); i++)
    {
        auto iter = m_World[i];
        // Check if the ray intersects the object's AABB
        std::pair<bool, float> intersect = mouseRay.intersects(iter->_getWorldAABB());
        if (intersect.first)
        {
            if (intersect.second < minDistance)
            {
                minDistance = intersect.second;
                closestNode = iter;
            }
        }
    }

    // If a closest node was found, you can change the selection
    if (closestNode)
    {
        (*m_CurrentlySelectedNode) = closestNode;
        g_OnSelectionChangedEvent.Invoke(&closestNode);
    }
}

bool CustomInput::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    if (evt.button == OgreBites::BUTTON_MIDDLE) {
        m_IsCameraMoving = false;
    }
    else if (evt.button == OgreBites::BUTTON_RIGHT) {
        m_IsRotating = false;
    }

    if (m_MoveHandles && evt.button == OgreBites::BUTTON_LEFT) {
        return m_MoveHandles->MouseReleased();
    }
    return true;
}

bool CustomInput::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    if (m_CameraNode) {
        // Move camera forward/backward along its view direction
        float moveSpeed = 10.0f;  // Adjust this value to control zoom speed
        Ogre::Vector3 forward = m_CameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;

        if (evt.y > 0) {
            // Move forward
            m_CameraNode->translate(forward * moveSpeed, Ogre::Node::TS_LOCAL);
        }
        else {
            // Move backward
            m_CameraNode->translate(-forward * moveSpeed, Ogre::Node::TS_LOCAL);
        }
        return true;
    }
    return false;
}
