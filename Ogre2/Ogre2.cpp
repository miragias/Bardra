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

    btDiscreteDynamicsWorld* dynamicsWorld;
    btRigidBody* boxRigidBody;
    MoveHandles* getMoveHandles() { return m_MoveHandles; }
    Ogre::Camera* Camera;
    Ogre::SceneNode* CamNode;
    Ogre::SceneManager* SceneManager;
    Ogre::ImGuiOverlay* ov;
    Ogre::SceneNode** m_CurrentlySelectedNode;
    std::vector<Ogre::SceneNode*> m_ObjectNodes;

private:
    Ogre::ManualObject* manual;
    MoveHandles* m_MoveHandles;
	int sliderValue = 0; // Variable to store the slider value
	float quadSize = 4.0f;
    std::vector<TimelineEvent> events;
    Timeline timeline;

protected:
    void setup() override
    {
        events = {};

        Timeline timeline = { 2.0f, 1.0f };


        // Call the base class setup
        OgreBites::ApplicationContext::setup();

        // Create the scene manager
        Ogre::SceneManager* scnMgr = getRoot()->createSceneManager();
        SceneManager = scnMgr;

        // Register the scene with the RTSS (Real-Time Shader System)
        Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(scnMgr);

        scnMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f)); // Add ambient light
        Ogre::Light* light = scnMgr->createLight("MainLight");
        Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->setPosition(0, 10, 15); // Position light
        lightNode->attachObject(light);

        // Set up the camera
        Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        camNode->setPosition(0, 0, 40);     // Position camera
        camNode->lookAt(Ogre::Vector3(0, 0, -5), Ogre::Node::TS_PARENT); // Look at origin
        CamNode = camNode;

        Ogre::Camera* cam = scnMgr->createCamera("myCam");
        Camera = cam;
        cam->setNearClipDistance(5);  // Adjust camera near clip distance
        cam->setAutoAspectRatio(true);  // Automatically adjust aspect ratio based on window size
        camNode->attachObject(cam);
        camNode->setPosition(0, 0, -10);

        // Attach the camera to the viewport
        getRenderWindow()->addViewport(cam);

        auto s1 = CreateEntity("S1");
        auto s2 = CreateEntity("S2");

        m_CurrentlySelectedNode = &s1;
        s2->setPosition(100, 0, 0);

        //imgui
        auto imguiOverlay = initialiseImGui();
        ov = imguiOverlay;

        //Add the input listeners
        addInputListener(this);
        addInputListener(getImGuiInputListener());

        float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();
        ImGui::GetIO().FontGlobalScale = std::round(vpScale); // default font does not work with fractional scaling

        imguiOverlay->setZOrder(300);
        imguiOverlay->show();
        imguiOverlay->NewFrame();
        scnMgr->addRenderQueueListener(mOverlaySystem);

        initBulletPhysics(scnMgr);

        m_MoveHandles = new MoveHandles(scnMgr, m_CurrentlySelectedNode, cam, boxRigidBody);
        createMaterialWithTexture();
        createTexturedQuad(scnMgr);
    }

    std::vector<Ogre::Vector3> vertices;

    void createTexturedQuad(Ogre::SceneManager* sceneMgr)
    {
        manual = sceneMgr->createManualObject("Quad");

        updateQuad();  // Build the quad initially

        // Create a scene node and attach the manual object
        Ogre::SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(manual);
        m_ObjectNodes.push_back(node);
    }

    void UpdateVertices() 
    {
        // Store initial vertex positions
        vertices = {
            {-quadSize, quadSize, 0},   // Top-left
            {quadSize, quadSize, 0},    // Top-right
            {-quadSize, -quadSize, 0},  // Bottom-left
            {quadSize, -quadSize, 0}    // Bottom-right
        };
    }

    void updateQuad()
    {
        UpdateVertices();
        manual->clear();  // Clear the previous shape

        manual->begin("Hey", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

        for (int i = 0; i < 4; i++) {
            manual->position(vertices[i]);   // Use updated vertex positions
            manual->textureCoord(i % 2, i / 2);
        }

        // Recreate the quad indices
        manual->index(0);
        manual->index(1);
        manual->index(2);
        manual->index(3);

        manual->end();
    }

    void createMaterialWithTexture()
    {
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
            "Hey", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
        pass->setCullingMode(Ogre::CULL_NONE);
        Ogre::TextureUnitState* texUnit = pass->createTextureUnitState("nm_bk.png");

        // Set material to use lighting (optional)
        material->setLightingEnabled(false);
    }

    Ogre::SceneNode* CreateEntity(const std::string name)
    {
        //TODO(JohnMir): Check if same name passed to fix this
        Ogre::SceneNode* node = SceneManager->getRootSceneNode()->createChildSceneNode(name);
        Ogre::Entity* entity = SceneManager->createEntity(Ogre::SceneManager::PT_SPHERE);

        node->attachObject(entity);
        node->setScale(1,1,1);

        //TODO:
        entity->setMaterialName("Examples/Rockwall");

        m_ObjectNodes.push_back(node);
        return node;
    }

    void initBulletPhysics(Ogre::SceneManager* scnMgr)
    {
        /*
        // 2. Bullet physics setup
        btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
        btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
        btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
        btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

        // 3. Create ground plane
        btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
        btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
        btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
        dynamicsWorld->addRigidBody(groundRigidBody);

        // 4. Create dynamic box in Bullet
        btCollisionShape* boxShape = new btBoxShape(btVector3(1, 1, 1));
        btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));
        btScalar mass = 1;
        btVector3 boxInertia(0, 0, 0);
        boxShape->calculateLocalInertia(mass, boxInertia);
        btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape, boxInertia);
        boxRigidBody = new btRigidBody(boxRigidBodyCI);
        dynamicsWorld->addRigidBody(boxRigidBody);
        */

        /*
        Ogre::Entity* boxEntity = scnMgr->createEntity(Ogre::SceneManager::PT_CUBE);
        boxNode = scnMgr->getRootSceneNode()->createChildSceneNode("BoxNode");
        boxNode->attachObject(boxEntity);
        boxNode->setScale(1, 1, 1); // Scale to match Bullet's unit cube
        */
    }

    void updateVertices(const Ogre::Vector3& topLeft = Ogre::Vector3(-1, 1, 0),
        const Ogre::Vector3& topRight = Ogre::Vector3(1, 1, 0),
        const Ogre::Vector3& bottomLeft = Ogre::Vector3(-1, -1, 0),
        const Ogre::Vector3& bottomRight = Ogre::Vector3(1, -1, 0))
    {
        std::cout << topLeft;
        manual->beginUpdate(0);

        // Front face vertices
        manual->position(topLeft);
        manual->textureCoord(0, 0);
        manual->position(topRight);
        manual->textureCoord(1, 0);
        manual->position(bottomLeft);
        manual->textureCoord(0, 1);
        manual->position(bottomRight);
        manual->textureCoord(1, 1);

        // Back face vertices
        manual->position(topLeft);
        manual->textureCoord(0, 0);
        manual->position(bottomLeft);
        manual->textureCoord(0, 1);
        manual->position(topRight);
        manual->textureCoord(1, 0);
        manual->position(bottomRight);
        manual->textureCoord(1, 1);

        // Front face indices
        manual->index(0);
        manual->index(1);
        manual->index(2);
        manual->index(2);
        manual->index(1);
        manual->index(3);

        // Back face indices
        manual->index(4);
        manual->index(5);
        manual->index(6);
        manual->index(6);
        manual->index(5);
        manual->index(7);

        manual->end();
    }

    void ShowSliderExample()
    {
        ImGui::Begin("Slider Example"); // Create a new window
        ImGui::SliderInt("Adjust Value", &sliderValue, -50, 50); // Create slider
        if (ImGui::SliderFloat("Quad Size", &quadSize, 0, 25)) 
        {
            updateQuad();
        }
        ImGui::Text("Current Value: %d", sliderValue); // Display value
        ImGui::End(); // End window
        ImGui::Begin("Timeline");

        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 150);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float timeline_length = canvas_size.x * timeline.zoom;
        float y_mid = canvas_pos.y + canvas_size.y / 2;

        // Draw timeline background
        ImGui::InvisibleButton("##timeline_button", canvas_size);
        bool is_hovered = ImGui::IsItemHovered();
        bool is_clicked = ImGui::IsItemClicked();

        draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255));

        // Draw time divisions (short and long lines)
        for (int i = 0; i <= 100; ++i) { // Increase division count for finer granularity
            float x = canvas_pos.x + (i / 100.0f) * timeline_length;
            float height = (i % 10 == 0) ? 15.0f : ((i % 5 == 0) ? 10.0f : 5.0f); // Different heights for major, minor, and tiny divisions
            draw_list->AddLine(ImVec2(x, y_mid - height), ImVec2(x, y_mid + height), IM_COL32(200, 200, 200, 255), 1.5f);
        }

        // Handle playhead movement on click
        if (is_clicked) {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            timeline.current_time = ((mouse_pos.x - canvas_pos.x) / timeline_length) * 10.0f; // Adjusted for 10 sec range
        }

        // Draw playhead (red vertical line)
        float playhead_x = canvas_pos.x + (timeline.current_time / 10.0f) * timeline_length;
        draw_list->AddLine(ImVec2(playhead_x, canvas_pos.y), ImVec2(playhead_x, canvas_pos.y + canvas_size.y), IM_COL32(255, 0, 0, 255), 2.0f);

        // Draw events (blocks on the timeline)
        for (const auto& event : events) {
            float start_x = canvas_pos.x + (event.start_time / 10.0f) * timeline_length;
            float end_x = start_x + (event.duration / 10.0f) * timeline_length;
            draw_list->AddRectFilled(ImVec2(start_x, y_mid - 10), ImVec2(end_x, y_mid + 10), IM_COL32(0, 255, 0, 255));
            ImGui::SetCursorScreenPos(ImVec2(start_x, y_mid + 15));
            ImGui::Text("%s", event.label.c_str());
        }

        ImGui::End();
    }

    bool frameRenderingQueued(const Ogre::FrameEvent& evt) override {
        /*
        // Simulate physics
        dynamicsWorld->stepSimulation(evt.timeSinceLastFrame, 10);

        // Get box transform from Bullet
        btTransform trans;
        boxRigidBody->getMotionState()->getWorldTransform(trans);
        btVector3 pos = trans.getOrigin();

        // Update Ogre node position
        bn->setPosition(Ogre::Vector3(pos.getX(), pos.getY(), pos.getZ()));
        */

        // Update move handles
        if (m_MoveHandles) 
        {
            m_MoveHandles->update();
        }

        //Imgui
        ov->NewFrame();
        ImGui::ShowDemoWindow();
        ShowSliderExample();
        return true;
    }

    struct TimelineEvent {
        std::string label;
        float time; // Time in range [0,1] representing position on timeline
    };


    OgreBites::NativeWindowPair createWindow(const Ogre::String& name, uint32_t w = 0, uint32_t h = 0,
        Ogre::NameValuePairList miscParams = Ogre::NameValuePairList()) override
    {
        return OgreBites::ApplicationContext::createWindow(name, 1600, 1200, miscParams);
    }
};

int main()
{
    CustomApplicationContext ctx("OgreTutorialApp");
    ctx.initApp();
    CustomInput* input = new CustomInput(&ctx, ctx.m_CurrentlySelectedNode, ctx.m_ObjectNodes);
    input->setMoveHandles(ctx.getMoveHandles());
    input->setCamera(ctx.Camera, ctx.CamNode, ctx.SceneManager);
    ctx.addInputListener(input);
    ctx.getRoot()->startRendering();
    ctx.closeApp();
    return 0;
}
