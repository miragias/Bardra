#include <Ogre.h>
#include <Bites/OgreApplicationContext.h>
#include <RTShaderSystem/OgreShaderGenerator.h>

#ifdef _DEBUG
#define OGRE_LIB_SUFFIX "_d"
#else
#define OGRE_LIB_SUFFIX ""
#endif

int main()
{
    OgreBites::ApplicationContext ctx("OgreTutorialApp");
    ctx.initApp();


    // Get a pointer to the already created root
    Ogre::Root* root = ctx.getRoot();


    // Create the scene manager
    Ogre::SceneManager* scnMgr = root->createSceneManager();

    // Register the scene with the RTSS (Real-Time Shader System)
    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);


    // Create a light source
    Ogre::Light* light = scnMgr->createLight("MainLight");
    Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->setPosition(0, 10, 15);  // Position light
    lightNode->attachObject(light);

    // Set up the camera
    Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(0, 0, 15);     // Position camera
    camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT); // Look at origin

    Ogre::Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5);  // Adjust camera near clip distance
    cam->setAutoAspectRatio(true);  // Automatically adjust aspect ratio based on window size
    camNode->attachObject(cam);

    // Attach the camera to the viewport
    ctx.getRenderWindow()->addViewport(cam);

    // Create the "Sinbad.mesh" entity and attach it to a scene node
    Ogre::Entity* ent = scnMgr->createEntity("Sinbad.mesh");
    Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);

    // Run the rendering loop
    root->startRendering();

    // Clean up the application (if needed, usually handled by ctx destructor)
    ctx.closeApp();

    return 0;
}