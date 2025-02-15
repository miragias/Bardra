#include "stdafx.h"
#include "MoveHandles.h"

const float MoveHandles::HANDLE_SCALE = 0.01f;
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
    m_HandleNode = m_SceneMgr->getRootSceneNode()->createChildSceneNode();

    // Create X axis handle (red)
    Ogre::Entity* xHandle = m_SceneMgr->createEntity("handle_x", Ogre::SceneManager::PT_CUBE);
    m_XHandle = m_HandleNode->createChildSceneNode();
    m_XHandle->attachObject(xHandle);
    m_XHandle->setScale(HANDLE_LENGTH, HANDLE_SCALE, HANDLE_SCALE);
    m_XHandle->setPosition(HANDLE_LENGTH / 2, 0, 0);
    xHandle->setMaterial(redMaterial);

    // Create Y axis handle (green)
    Ogre::Entity* yHandle = m_SceneMgr->createEntity("handle_y", Ogre::SceneManager::PT_CUBE);
    m_YHandle = m_HandleNode->createChildSceneNode();
    m_YHandle->attachObject(yHandle);
    m_YHandle->setScale(HANDLE_SCALE, HANDLE_LENGTH, HANDLE_SCALE);
    m_YHandle->setPosition(0, HANDLE_LENGTH / 2, 0);
    yHandle->setMaterial(greenMaterial);
}

void MoveHandles::Update() {
    if (*m_TargetNode != nullptr) 
    {
        m_HandleNode->setPosition((*m_TargetNode)->getPosition());
    }
}

void MoveHandles::onSelectionChanged(SelectionMode currentMode)
{
    /*
    std::cout << "HAPPENED";
    setVisible(currentMode == SelectionMode::OBJECT);
    */
}

MoveHandles::Axis MoveHandles::getSelectedAxis(const Ogre::Vector2& mousePos) {
    Ogre::Ray mouseRay = m_Camera->getCameraToViewportRay(
        mousePos.x,
        mousePos.y
    );

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

Ogre::Vector3 MoveHandles::getMouseWorldPos(const Ogre::Vector2& mousePos, const Ogre::Plane& plane) {
    Ogre::Ray mouseRay = m_Camera->getCameraToViewportRay(mousePos.x, mousePos.y);
    std::pair<bool, float> result = mouseRay.intersects(plane);
    if (result.first) {
        return mouseRay.getPoint(result.second);
    }
    return Ogre::Vector3::ZERO;
}

bool MoveHandles::MousePressed(const Ogre::Vector2& mousePos) {
    m_SelectedAxis = getSelectedAxis(mousePos);
    if (m_SelectedAxis != None) {
        m_IsDragging = true;

        // Use the same plane calculation as mouseMove
        Ogre::Vector3 cameraDir = m_Camera->getDerivedDirection();
        Ogre::Vector3 moveDir;

        switch (m_SelectedAxis) {
        case X: moveDir = Ogre::Vector3::UNIT_X; break;
        case Y: moveDir = Ogre::Vector3::UNIT_Y; break;
        case Z: moveDir = Ogre::Vector3::UNIT_Z; break;
        default: return false;
        }

        // Create the same plane as in mouseMove
        Ogre::Vector3 planeNormal = moveDir.crossProduct(cameraDir);
        planeNormal = planeNormal.crossProduct(moveDir);
        planeNormal.normalise();

        Ogre::Plane dragPlane(planeNormal, (*m_TargetNode)->getPosition());
        m_LastMousePos = getMouseWorldPos(mousePos, dragPlane);

        return true;
    }
    return false;
}

void MoveHandles::MouseMove(const Ogre::Vector2& mousePos) {
    if (!m_IsDragging || m_SelectedAxis == None) return;

    // Create a plane perpendicular to the camera's view direction
    Ogre::Vector3 cameraDir = m_Camera->getDerivedDirection();
    Ogre::Vector3 moveDir;

    switch (m_SelectedAxis) {
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

    Ogre::Plane dragPlane(planeNormal, (*m_TargetNode)->getPosition());
    Ogre::Vector3 currentPos = getMouseWorldPos(mousePos, dragPlane);

    if (currentPos != Ogre::Vector3::ZERO) {
        Ogre::Vector3 delta = currentPos - m_LastMousePos;
        float projection = delta.dotProduct(moveDir);
        Ogre::Vector3 movement = moveDir * projection;
		(*m_TargetNode)->setPosition((*m_TargetNode)->getPosition() + movement);
        //TODO(JohnMir): Use the mode here we do not need the if
        auto it = m_VertexNodeToIndex->find(*m_TargetNode);
        if(it != m_VertexNodeToIndex->end())
        {
            int index = (*m_VertexNodeToIndex)[*m_TargetNode];
            (*m_Vertices)[index] = (*m_TargetNode)->getPosition();
        }
        m_LastMousePos = currentPos;
    }
}

bool MoveHandles::MouseReleased() {
    if (m_IsDragging) {
        m_IsDragging = false;
        m_SelectedAxis = None;
        return true;
    }
    return false;
}

void MoveHandles::setVisible(bool visible) {
    m_HandleNode->setVisible(visible);
}
