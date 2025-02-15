#pragma once
#include "stdafx.h"

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreShaderGenerator.h>

#include <LinearMath/btVector3.h>
#include <LinearMath/btDefaultMotionState.h>

#include <imgui.h>
#include <OgreImGuiOverlay.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>
#include <OgreImGuiInputListener.h>
#include "MoveHandles.h"
#include "CustomInput.h"
#include "SelectionContext.h"
#include "Events.h"


struct TimelineEvent {
    std::string Label;
    float StartTime; // Start time in seconds
    float Duration;   // Duration in seconds
};


struct Timeline {
    float CurrentTime = 0.0f; // Current playhead position
    float Zoom = 1.0f; // Zoom level (1.0 = normal, >1 = zoom in, <1 = zoom out)
};
class CustomApplicationContext : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
public:
    CustomApplicationContext(const Ogre::String& appName) : OgreBites::ApplicationContext(appName) {}

private:
    void SetCurrentlySelected(Ogre::SceneNode** newNode);


    std::vector<Ogre::SceneNode*> GetWorld() const;
    MoveHandles* getMoveHandles() { return m_MoveHandles; }
    Ogre::Camera* Camera;
    Ogre::SceneNode* CamNode;
    Ogre::SceneManager* SceneManager;
    Ogre::ImGuiOverlay* ImguiOverlayContext;
    Ogre::SceneNode** CurrentlySelectedNode;

    SelectionContext* m_SelectionContext;
    Ogre::SceneNode* m_VerticesNodesParent;
    std::vector<Ogre::SceneNode*> m_ObjectNodes;
    void resizeQuad();
    Ogre::ManualObject* m_ManualObject;
    MoveHandles* m_MoveHandles;
    CustomInput* m_CustomInput;
    std::vector<TimelineEvent> m_TimelineEvents;
	int m_SliderValue = 0; // Variable to store the slider value
	float m_QuadSize = 4.0f;
    Timeline m_Timeline;
    std::vector<Ogre::Vector3> m_Vertices;
    std::vector<Ogre::Vector2> m_TextureCoords;
    std::vector<int> m_Indices;
    SelectionMode m_CurrentSelectionMode;

    void CheckDoAppropriateSystem(SelectionMode selectionMode);
    void DestroySceneNode(Ogre::SceneNode* node);
    void createVertexNodes();
    void addVertexToQuad(const Ogre::Vector3& vertex,
                         const Ogre::Vector2& texCoord,
                         const std::array<int,2> otherVertexIndices,
                         size_t index);
    void SetSelectionTo(SelectionMode selectionToSetTo);
    Ogre::SceneNode* CreateEntity(const std::string name, const Ogre::Vector3* targetPosition = nullptr,
                                  Ogre::SceneNode* parent = nullptr,
                                  Ogre::Entity* entityToAttach = nullptr,
                                  const Ogre::Vector3* targetScale = nullptr);

protected:

    void setup() override;
    Ogre::SceneNode* createTexturedQuad(Ogre::SceneManager* sceneMgr);
    void ClearBuffersAndCreateDefault() ;
    void updateQuad();
    void createMaterialWithTexture();
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
