#include <Ogre.h>
#include <Bites/OgreApplicationContext.h>
#include <RTShaderSystem/OgreShaderGenerator.h>
#include "CustomInput.h"

class CustomApplicationContext : public OgreBites::ApplicationContext
{
public:
    CustomApplicationContext(const Ogre::String& appName) : OgreBites::ApplicationContext(appName) {}

protected:
    void setup() override
    {
        // Call the base class setup
        OgreBites::ApplicationContext::setup();

        // Create the scene manager
        Ogre::SceneManager* scnMgr = getRoot()->createSceneManager();

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
        getRenderWindow()->addViewport(cam);

        // Create the "Sinbad.mesh" entity and attach it to a scene node
        Ogre::Entity* ent = scnMgr->createEntity("Sinbad.mesh");
        Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(ent);
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
    ctx.addInputListener(new CustomInput(&ctx));
    ctx.getRoot()->startRendering();
    ctx.closeApp();
    return 0;
}
