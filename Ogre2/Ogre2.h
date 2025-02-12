#pragma once
#include "stdafx.h"

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreShaderGenerator.h>

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btDefaultMotionState.h>

#include <imgui.h>
#include <OgreImGuiOverlay.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>
#include <OgreImGuiInputListener.h>
#include "MoveHandles.h"
#include "CustomInput.h"
#include "Events.h"


struct TimelineEvent {
    std::string label;
    float start_time; // Start time in seconds
    float duration;   // Duration in seconds
};

struct Timeline {
    float current_time = 0.0f; // Current playhead position
    float zoom = 1.0f; // Zoom level (1.0 = normal, >1 = zoom in, <1 = zoom out)
};
class CustomApplicationContext : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
public:
    CustomApplicationContext(const Ogre::String& appName) : OgreBites::ApplicationContext(appName) {}
    void SetCurrentlySelected(Ogre::SceneNode** newNode);
    btDiscreteDynamicsWorld* dynamicsWorld;
    btRigidBody* boxRigidBody;
    MoveHandles* getMoveHandles() { return m_MoveHandles; }
    Ogre::Camera* Camera;
    Ogre::SceneNode* CamNode;
    Ogre::SceneManager* SceneManager;
    Ogre::ImGuiOverlay* ov;
    Ogre::SceneNode** CurrentlySelectedNode;
    std::vector<Ogre::SceneNode*> GetWorld() const;

private:
    std::vector<Ogre::SceneNode*> m_ObjectNodes;
    void ResizeQuad();
    Ogre::ManualObject* manual;
    MoveHandles* m_MoveHandles;
	int sliderValue = 0; // Variable to store the slider value
	float quadSize = 4.0f;
    std::vector<TimelineEvent> events;
    Timeline timeline;
    std::vector<Ogre::Vector3> vertices;
    std::vector<Ogre::Vector2> textureCoords;
    std::vector<int> indices;
    SelectionMode m_CurrentSelectionMode;

    void addVertexToQuad(const Ogre::Vector3& vertex,
                         const Ogre::Vector2& texCoord,
                         const std::array<int,2> otherVertexIndices,
                         size_t index);
    void SetSelectionTo(SelectionMode selectionToSetTo);

protected:

    void setup() override;
    Ogre::SceneNode* createTexturedQuad(Ogre::SceneManager* sceneMgr);
    void ClearBuffersAndCreateDefault() ;
    void updateQuad();
    void createMaterialWithTexture();
    Ogre::SceneNode* CreateEntity(const std::string name);
    void initBulletPhysics(Ogre::SceneManager* scnMgr);

    void updateVertices(const Ogre::Vector3& topLeft = Ogre::Vector3(-1, 1, 0),
        const Ogre::Vector3& topRight = Ogre::Vector3(1, 1, 0),
        const Ogre::Vector3& bottomLeft = Ogre::Vector3(-1, -1, 0),
        const Ogre::Vector3& bottomRight = Ogre::Vector3(1, -1, 0));

    void ShowSliderExample();

    bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
    
    OgreBites::NativeWindowPair createWindow(const Ogre::String& name, uint32_t w = 0, uint32_t h = 0,
                                             Ogre::NameValuePairList miscParams = Ogre::NameValuePairList())
    {
        return OgreBites::ApplicationContext::createWindow(name, 1600, 1200, miscParams);
    }
};
