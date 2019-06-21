#include "testapp.hh"

#include "Terrain.hh"

static Ogre::ManualObject* createCubeMesh(Ogre::String name, Ogre::String matName, Ogre::Real size = 0.5)
{
  auto cube = OGRE_NEW Ogre::ManualObject(name);
  cube->begin(matName);

  cube->position(size,-size,size);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,0);
  cube->position(-size,-size, -size);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,1);
  cube->position(size,-size, -size);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,1);
  cube->position(-size,-size,size);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,0);
  cube->position(size,size,size);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(1,0);
  cube->position(-size,-size,size);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,1);
  cube->position(size,-size,size);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,1);
  cube->position(-size,size,size);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(0,0);
  cube->position(-size,size, -size);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(0,1);
  cube->position(-size,-size, -size);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(1,1);
  cube->position(-size,-size,size);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(1,0);
  cube->position(size,-size, -size);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(0,1);
  cube->position(size,size, -size);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(1,1);
  cube->position(size,-size,size);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(0,0);
  cube->position(size,-size, -size);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,0);
  cube->position(-size,-size, -size);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,0);
  cube->position(-size,size,size);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(1,0);
  cube->position(size,size, -size);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(0,1);
  cube->position(-size,size, -size);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(1,1);
  cube->position(size,size,size);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(0,0);

  cube->triangle(0,2,1);      cube->triangle(3,0,1);
  cube->triangle(4,6,5);      cube->triangle(4,5,7);
  cube->triangle(8,10,9);      cube->triangle(10,8,7);
  cube->triangle(4,12,11);   cube->triangle(4,11,13);
  cube->triangle(14,12,8);   cube->triangle(14,8,15);
  cube->triangle(16,18,17);   cube->triangle(16,17,19);
  cube->end();

  return cube;
}


static void CreateGrid(Ogre::Real distance, int size,
                       Ogre::SceneManager *mgr,
                       const Ogre::MeshPtr mesh)
{
  for (int x = 0; x < size; ++x)
    for (int y = 0; y < size; ++y)
      for (int z = 0; z < size; ++z)
        {
          std::stringstream name;
          name << "GridCube" << x << "," << y << "," << z;
          Ogre::Entity *ent = mgr->createEntity(name.str(), mesh);
          Ogre::SceneNode *gnode = mgr->getRootSceneNode()
            ->createChildSceneNode(Ogre::Vector3(x * distance * 10, y * distance, z * distance * 10));
          gnode->attachObject(ent);
        }
}


void OgreCardboardTestApp::setupCamera()
{
  forBothCameras([](Ogre::Camera *cam){
      cam->setNearClipDistance(10.0f);
      cam->setFarClipDistance(1000.0f);
    });
}

#if defined(ANDROID)
void OgreCardboardTestApp::setupResources(Ogre::ResourceGroupManager &rgm)
{
  rgm.createResourceGroup("Terrain/Default", false);
  rgm.addResourceLocation("/terrain", "APKFileSystem", "Terrain/Default");
  rgm.addResourceLocation("/models", "APKFileSystem");
}
#else
void OgreCardboardTestApp::setupResources(Ogre::ResourceGroupManager &rgm)
{
  rgm.createResourceGroup("Terrain/Default", false);
  rgm.addResourceLocation("../project/assets/terrain", "FileSystem", "Terrain/Default");
  rgm.addResourceLocation("../project/assets/models", "FileSystem");
}

#endif

void OgreCardboardTestApp::initialize()
{
  OgreCardboardApp::initialize();

  forBothCamerasAndViewports([](Ogre::Camera *c, Ogre::Viewport *vp){
    c->setPosition(Ogre::Vector3(80.0f, 80.0f, 80.0f));
    c->lookAt(Ogre::Vector3(0.0f, 80.0f, 0.0f));
    vp->setBackgroundColour(Ogre::ColourValue::Black);
    });

  Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
  lightdir.normalise();
  Ogre::Light *l = sceneManager->createLight("tstLight");
  l->setType(Ogre::Light::LT_DIRECTIONAL);
  l->setDirection(lightdir);
  l->setDiffuseColour(Ogre::ColourValue::White);
  l->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

  sceneManager->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

  Ogre::HighLevelGpuProgramManager &programMgr = Ogre::HighLevelGpuProgramManager::getSingleton();
  programMgr.setVerbose(true);
  Ogre::HighLevelGpuProgramPtr vertexProgram =
    programMgr.createProgram("vTest", "General", "glsles", Ogre::GPT_VERTEX_PROGRAM);
  vertexProgram->setSource(VertexShader);
  vertexProgram->setVertexTextureFetchRequired(false);

  Ogre::HighLevelGpuProgramPtr fragmentProgram =
    programMgr.createProgram("fTest", "General", "glsles", Ogre::GPT_FRAGMENT_PROGRAM);
  fragmentProgram->setSource(FragmentShader);

  auto scriptStream = OGRE_NEW Ogre::MemoryDataStream("materialStream",
                                                      (void*)ShaderMaterialScript,
                                                      strlen(ShaderMaterialScript));
  Ogre::SharedPtr<Ogre::DataStream> ptr =
    Ogre::SharedPtr<Ogre::DataStream>(scriptStream, Ogre::SPFM_DELETE);
  Ogre::MaterialManager::getSingleton().parseScript(ptr, "General");

  Ogre::ManualObject *cube = createCubeMesh("Cube", "myshadermaterial");
  Ogre::MeshPtr cube_mesh = cube->convertToMesh("CubeMesh");

  Ogre::Entity* column_ent = sceneManager->createEntity("column.mesh");
  column_ent->setMaterialName("myshadermaterial");
  mNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  mNode->attachObject(column_ent);
  mNode->setPosition(0.0f, 0.0f, 0.0f);
  mNode->setScale(0.02f, 0.02f, 0.02f);

  CreateGrid(30.0, 3, sceneManager, cube_mesh);

  terrain = new ::Terrain(root, "Terrain/Default" /*DefaultTerrainResourceGroup*/);

  camNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  camNode->setPosition(lcam->getDerivedPosition());


  Ogre::ManualObject *small_cube = createCubeMesh("Cube", "myshadermaterial", 0.03);
  Ogre::MeshPtr small_cube_mesh = small_cube->convertToMesh("SmallCubeMesh");
  small_cube_mesh->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-10, -10, -10),
                                                   Ogre::Vector3(10, 10, 10)));
  Ogre::Entity *nf_ent = sceneManager->createEntity("NearFieldCube", small_cube_mesh);
  column_ent->setMaterialName("myshadermaterial");
  nfNode = camNode->createChildSceneNode(Ogre::Vector3(0, 0, 0.5));
  nfNode->attachObject(nf_ent);
  nf_ent->setRenderQueueGroup(51);

  sceneManager->addRenderQueueListener(this);
}

void OgreCardboardTestApp::mainLoop()
{
  if (forward)
    {
      Ogre::Vector3 d(lcam->getDerivedDirection());
      forBothCameras([&d](Ogre::Camera *cam){cam->move(d *  0.5);});
    }
  else if (backward)
    {
      Ogre::Vector3 d(lcam->getDerivedDirection());
      forBothCameras([&d](Ogre::Camera *cam){cam->move(d * -0.5);});
    }

  if (left)
    forBothCameras([](Ogre::Camera *cam){cam->yaw(Ogre::Radian( 0.05));});
  else if (right)
    forBothCameras([](Ogre::Camera *cam){cam->yaw(Ogre::Radian(-0.05));});

  auto n = lcam->getDerivedPosition();
  camNode->setPosition(n);
  camNode->yaw(Ogre::Radian(0.1));
  nfNode->yaw(Ogre::Radian(0.5));
  nfNode->pitch(Ogre::Radian(0.05));

  mNode->translate(Ogre::Vector3(0.1, 0.1, 0.1));
  mNode->yaw(Ogre::Radian(0.05));
  mNode->pitch(Ogre::Radian(0.05));
  if (mNode->getPosition().z > 100.0)
    mNode->setPosition(0.0f, 0.0f, 0.0f);

  renderFrame();
}

void OgreCardboardTestApp::handleKeyDown(int key)
{
  switch(key)
    {
    case 1:
      left = true;
      break;
    case 2:
      right = true;
      break;
    case 3:
      forward = true;
      break;
    case 4:
      backward = true;
      break;
    default:
      break;
    }
}

void OgreCardboardTestApp::handleKeyUp(int key)
{
  switch(key)
    {
    case 1:
      left = false;
      break;
    case 2:
      right = false;
      break;
    case 3:
      forward = false;
      break;
    case 4:
      backward = false;
      break;
    default:
      break;
    }
}

void OgreCardboardTestApp::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation)
{
  if (queueGroupId == 51)
    {
      auto rs = root->getRenderSystem();
      auto vp = rs->_getViewport();
      auto cam = vp->getCamera();

      cam->setNearClipDistance(0.2f);
      cam->setFarClipDistance(5.0f);
#if defined(ANDROID)
      gvr::Rectf fp = OgreCardboardApp::GVRFOV2FrustumExtents(cam == lcam ? lFOV : rFOV, cam->getNearClipDistance());
      cam->setFrustumExtents(fp.left, fp.right, fp.top, fp.bottom);
#endif
      rs->_setProjectionMatrix(cam->getProjectionMatrixRS());
      vp->clear(Ogre::FrameBufferType::FBT_DEPTH);
    }
}

void OgreCardboardTestApp::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation)
{
  if (queueGroupId == 51)
    {
      auto rs = root->getRenderSystem();
      auto cam = rs->_getViewport()->getCamera();
      cam->setNearClipDistance(10.0f);
      cam->setFarClipDistance(1000.0f);
#if defined(ANDROID)
      gvr::Rectf fp = OgreCardboardApp::GVRFOV2FrustumExtents(cam == lcam ? lFOV : rFOV, cam->getNearClipDistance());
      cam->setFrustumExtents(fp.left, fp.right, fp.top, fp.bottom);
#endif
      root->getRenderSystem()->_setProjectionMatrix(cam->getProjectionMatrixRS());
    }
}
