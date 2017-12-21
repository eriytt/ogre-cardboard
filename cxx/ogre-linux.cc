#include "ogre-linux.hh"

OgreCardboardApp::OgreCardboardApp(): OgreApp()
{
}

void OgreCardboardApp::initialize()
{
  root = OGRE_NEW Ogre::Root("plugins.cfg");
  Ogre::RenderSystem *rs = root->getRenderSystemByName("OpenGL Rendering Subsystem");
  root->setRenderSystem(rs);
  rs->setConfigOption("Full Screen", "No");
  rs->setConfigOption("Video Mode", "1920x1080 @ 32-bit colour");

  renderWindow = root->initialise(true);

  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();
  // Call concrete implementation
  setupResources(rgm);
  rgm.initialiseAllResourceGroups();

  sceneManager = root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
  lcam = sceneManager->createCamera("LeftEye");
  rcam = sceneManager->createCamera("RightEye");

  lcam->setFrustumOffset( 0.032f, 0.0f);
  rcam->setFrustumOffset(-0.032f, 0.0f);

  setupCamera();

  lviewport = renderWindow->addViewport(lcam, 0, 0.0f, 0.0f, 0.5f, 1.0f);
  rviewport = renderWindow->addViewport(rcam, 1, 0.5f, 0.0f, 0.5f, 1.0f);

  initialized = true;
}

size_t OgreCardboardApp::getWindowHandle()
{
  if (not initialized)
    throw OgreAppError("OgreCardboardApp has not been initialized");

  size_t windowHnd = 0;
  renderWindow->getCustomAttribute("WINDOW", &windowHnd);
  return windowHnd;
}

void OgreCardboardApp::renderFrame()
{
  Ogre::WindowEventUtilities::messagePump();
  root->renderOneFrame();
}
