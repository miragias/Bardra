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
    Ogre::SceneNode* m_CurrentlySelectedNode;
    std::vector<Ogre::SceneNode*> m_ObjectNodes;

private:
    MoveHandles* m_MoveHandles;
	int sliderValue = 0; // Variable to store the slider value

protected:
    void setup() override
    {
        // Call the base class setup
        OgreBites::ApplicationContext::setup();

        // Create the scene manager
        Ogre::SceneManager* scnMgr = getRoot()->createSceneManager();
        SceneManager = scnMgr;

        // Register the scene with the RTSS (Real-Time Shader System)
        Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(scnMgr);

        // Create a light source
        /*
        Ogre::Light* light = scnMgr->createLight("MainLight");
        Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->setPosition(0, 10, 15);  // Position light
        lightNode->attachObject(light);
        */

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

        m_CurrentlySelectedNode = s1;
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

    void ShowSliderExample()
    {
        ImGui::Begin("Slider Example"); // Create a new window
        ImGui::SliderInt("Adjust Value", &sliderValue, -50, 50); // Create slider
        ImGui::Text("Current Value: %d", sliderValue); // Display value
        ImGui::End(); // End window
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
