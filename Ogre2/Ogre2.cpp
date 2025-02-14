#include "stdafx.h"
#include "MoveHandles.h"
#include "CustomInput.h"
#include "Events.h"
#include "Ogre2.h"
#include "Common.h"


std::vector<Ogre::SceneNode*> CustomApplicationContext::GetWorld() const{
    return m_ObjectNodes;
}

void CustomApplicationContext::setup()
{
    m_TimelineEvents = {};
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

    /*
    auto s1 = CreateEntity("S1");
    auto s2 = CreateEntity("S2");

    CurrentlySelectedNode = &s1;
    s2->setPosition(100, 0, 0);
    */

    //imgui
    auto imguiOverlay = initialiseImGui();
    ImguiOverlayContext = imguiOverlay;

    //Add the input listeners
    addInputListener(this);
    addInputListener(getImGuiInputListener());

    float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();
    ImGui::GetIO().FontGlobalScale = std::round(vpScale); // default font does not work with fractional scaling

    imguiOverlay->setZOrder(300);
    imguiOverlay->show();
    imguiOverlay->NewFrame();
    scnMgr->addRenderQueueListener(mOverlaySystem);

    createMaterialWithTexture();
    Ogre::SceneNode* quadNode = createTexturedQuad(scnMgr);
    CurrentlySelectedNode = &quadNode;

    m_MoveHandles = new MoveHandles(scnMgr, CurrentlySelectedNode, cam, &m_CurrentSelectionMode);

    g_OnSelectionChangedEvent.Subscribe([this](Ogre::SceneNode** newNode) {
        SetCurrentlySelected(newNode);
	});
}

void CustomApplicationContext::SetCurrentlySelected(Ogre::SceneNode** newNode) 
{
    CurrentlySelectedNode = newNode;
}


Ogre::SceneNode* CustomApplicationContext::createTexturedQuad(Ogre::SceneManager* sceneMgr)
{
    m_ManualObject = sceneMgr->createManualObject("Quad");

    ClearBuffersAndCreateDefault();
    updateQuad();

    // Create a scene node and attach the manual object
    Ogre::SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(m_ManualObject);
    m_ObjectNodes.push_back(node);
    return node;
}

void CustomApplicationContext::addVertexToQuad(const Ogre::Vector3& vertex,
                                               const Ogre::Vector2& texCoord,
                                               const std::array<int,2> otherVertexIndices,
                                               size_t index)
{
    if (index > m_Vertices.size()) 
    {
        index = m_Vertices.size();
    }
    m_Vertices.insert(m_Vertices.begin() + index, vertex);
    m_TextureCoords.insert(m_TextureCoords.begin() + index, texCoord);

    //TODO(JohnMir): Improve this
    if(otherVertexIndices[0] == -1) return;

    //The indexes of the other 2 vertexes which are neighbours to the new incoming index
    for(int index : otherVertexIndices)
    {
        m_Indices.push_back(index);
    }
    m_Indices.push_back(index); //The new index
}

void CustomApplicationContext::SetSelectionTo(SelectionMode selectionToSetTo)
{
    m_CurrentSelectionMode = selectionToSetTo;
    g_OnSelectionModeChangedEvent.Invoke(m_CurrentSelectionMode);
}

void CustomApplicationContext::ClearBuffersAndCreateDefault() 
{
    // Clear previous vertices
    m_Vertices.clear();
    m_TextureCoords.clear();
    m_Indices.clear();

    // Default quad vertices
    addVertexToQuad(Ogre::Vector3(-m_QuadSize, m_QuadSize, 0), Ogre::Vector2(0, 0.5f), {-1,-1} , m_Vertices.size());   // Top-left
    addVertexToQuad(Ogre::Vector3(m_QuadSize, m_QuadSize, 0), Ogre::Vector2(1,0.5f), {-1, -1}, m_Vertices.size());    // Top-right
    addVertexToQuad(Ogre::Vector3(-m_QuadSize, -m_QuadSize, 0), Ogre::Vector2(0,0), {-1, -1}, m_Vertices.size());  // Bottom-left
    addVertexToQuad(Ogre::Vector3(m_QuadSize, -m_QuadSize, 0), Ogre::Vector2(1, 0), {-1 , -1}, m_Vertices.size());   // Bottom-right
    
    // Define triangles dynamically based on vertex order
    m_Indices.push_back(0);
    m_Indices.push_back(1); // New vertex in the middle top
    m_Indices.push_back(2);
    
    m_Indices.push_back(1);
    m_Indices.push_back(2);
    m_Indices.push_back(3);

}

void CustomApplicationContext::resizeQuad()
{
    m_Vertices[0] = Ogre::Vector3(-m_QuadSize, m_QuadSize, 0);
    m_Vertices[1] = Ogre::Vector3(m_QuadSize, m_QuadSize, 0);
    m_Vertices[2] = Ogre::Vector3(-m_QuadSize, -m_QuadSize, 0);
    m_Vertices[3] = Ogre::Vector3(m_QuadSize, -m_QuadSize, 0);
}

void CustomApplicationContext::updateQuad()
{
    m_ManualObject->clear();  // Clear previous shape
    m_ManualObject->begin("Hey", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    
    for (size_t i = 0; i < m_Vertices.size(); ++i) {
        m_ManualObject->position(m_Vertices[i]);   // Use updated vertex positions
        m_ManualObject->textureCoord(m_TextureCoords[i]);
    }

    for (size_t i = 0; i < m_Indices.size(); ++i) 
    {
        m_ManualObject->index(m_Indices[i]);
    }

    m_ManualObject->end();
}

void CustomApplicationContext::createMaterialWithTexture()
{
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        "Hey", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
    pass->setCullingMode(Ogre::CULL_NONE);
    Ogre::TextureUnitState* texUnit = pass->createTextureUnitState("nm_bk.png");

    // Set material to use lighting (optional)
    material->setLightingEnabled(false);
}

Ogre::SceneNode* CustomApplicationContext::CreateEntity(const std::string name)
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

void CustomApplicationContext::ShowSliderExample()
{
    ImGui::Begin("Slider Example"); // Create a new window
    ImGui::SliderInt("Adjust Value", &m_SliderValue, -50, 50); // Create slider
    if (ImGui::SliderFloat("Quad Size", &m_QuadSize, 0, 25)) 
    {
        ClearBuffersAndCreateDefault();
        updateQuad();
    }
    if (ImGui::Button("Add vertex on top side")) 
    {
        addVertexToQuad(Ogre::Vector3(0, m_QuadSize + 2, 0), Ogre::Vector2(0.5, 1), {0,1}, 4);
        updateQuad();
    }
    ImGui::Text("Current Value: %d", m_SliderValue); // Display value
    ImGui::End(); // End window
    ImGui::Begin("Timeline");

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 150);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float timeline_length = canvas_size.x * m_Timeline.Zoom;
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
        m_Timeline.CurrentTime = ((mouse_pos.x - canvas_pos.x) / timeline_length) * 10.0f; // Adjusted for 10 sec range
    }

    // Draw playhead (red vertical line)
    float playhead_x = canvas_pos.x + (m_Timeline.CurrentTime / 10.0f) * timeline_length;
    draw_list->AddLine(ImVec2(playhead_x, canvas_pos.y), ImVec2(playhead_x, canvas_pos.y + canvas_size.y), IM_COL32(255, 0, 0, 255), 2.0f);

    // Draw events (blocks on the timeline)
    for (const auto& event : m_TimelineEvents) {
        float start_x = canvas_pos.x + (event.StartTime / 10.0f) * timeline_length;
        float end_x = start_x + (event.Duration / 10.0f) * timeline_length;
        draw_list->AddRectFilled(ImVec2(start_x, y_mid - 10), ImVec2(end_x, y_mid + 10), IM_COL32(0, 255, 0, 255));
        ImGui::SetCursorScreenPos(ImVec2(start_x, y_mid + 15));
        ImGui::Text("%s", event.Label.c_str());
    }

    //Change mode
    if (ImGui::RadioButton("Vertex", m_CurrentSelectionMode == SelectionMode::VERTEX))
        SetSelectionTo(SelectionMode::VERTEX);
    if (ImGui::RadioButton("Object", m_CurrentSelectionMode == SelectionMode::OBJECT))
        SetSelectionTo(SelectionMode::OBJECT);

    ImGui::End();
}

bool CustomApplicationContext::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
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
        m_MoveHandles->Update();
    }

    //Imgui
    ImguiOverlayContext->NewFrame();
    ImGui::ShowDemoWindow();
    ShowSliderExample();
    return true;
}

int main()
{
    CustomApplicationContext ctx("OgreTutorialApp");
    ctx.initApp();

    CustomInput* input = new CustomInput(&ctx);
    ctx.addInputListener(input);
    ctx.getRoot()->startRendering();
    ctx.closeApp();
    return 0;
}
