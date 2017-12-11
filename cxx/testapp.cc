#include "testapp.hh"

#include "Terrain.hh"

static Ogre::ManualObject* createCubeMesh(Ogre::String name, Ogre::String matName)
{
  auto cube = OGRE_NEW Ogre::ManualObject(name);
  cube->begin(matName);

  cube->position(0.5,-0.5,0.5);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,0);
  cube->position(-0.5,-0.5, -0.5);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,1);
  cube->position(0.5,-0.5, -0.5);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,1);
  cube->position(-0.5,-0.5,0.5);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,0);
  cube->position(0.5,0.5,0.5);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(1,0);
  cube->position(-0.5,-0.5,0.5);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,1);
  cube->position(0.5,-0.5,0.5);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,1);
  cube->position(-0.5,0.5,0.5);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(0,0);
  cube->position(-0.5,0.5, -0.5);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(0,1);
  cube->position(-0.5,-0.5, -0.5);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(1,1);
  cube->position(-0.5,-0.5,0.5);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(1,0);
  cube->position(0.5,-0.5, -0.5);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(0,1);
  cube->position(0.5,0.5, -0.5);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(1,1);
  cube->position(0.5,-0.5,0.5);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(0,0);
  cube->position(0.5,-0.5, -0.5);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,0);
  cube->position(-0.5,-0.5, -0.5);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,0);
  cube->position(-0.5,0.5,0.5);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(1,0);
  cube->position(0.5,0.5, -0.5);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(0,1);
  cube->position(-0.5,0.5, -0.5);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(1,1);
  cube->position(0.5,0.5,0.5);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(0,0);

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
            ->createChildSceneNode(Ogre::Vector3(x * distance, y * distance, z * distance));
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

void OgreCardboardTestApp::setupResources(Ogre::ResourceGroupManager &rgm)
{
  rgm.createResourceGroup("Terrain/Default", false);
  rgm.addResourceLocation("/terrain", "APKFileSystem", "Terrain/Default");
  rgm.addResourceLocation("/models", "APKFileSystem");
}

void OgreCardboardTestApp::initialize(JNIEnv *env, jobject androidSurface, gvr_context *gvr, AAssetManager* amgr)
{
  OgreCardboardApp::initialize(env, androidSurface, gvr, amgr);

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
