#include "Terrain.hh"

#include <Ogre.h>
#include <OgreTerrainMaterialGeneratorA.h>

using namespace Ogre;

::Terrain::Terrain(Root *root, const Ogre::String &resourceGroupName)
  : mTerrainGlobals(0), mTerrainGroup(0), mTerrainsImported(false), resourceGroup(resourceGroupName)
{
  SceneManager *mgr = root->getSceneManager("SceneManager");
  heightScale = 60.0;
  setupContent(mgr);
}

::Terrain::~Terrain()
{
  if (mTerrainGroup)
    delete mTerrainGroup;
  if (mTerrainGlobals)
    delete mTerrainGlobals;
}

void ::Terrain::saveTerrains(bool onlyIfModified)
{
  mTerrainGroup->saveAllTerrains(onlyIfModified);
}

void ::Terrain::defineTerrain(long x, long y, bool flat)
{
  if (flat)
    {
      mTerrainGroup->defineTerrain(x, y, 0.0f);
    }
  else
    {
      String filename = mTerrainGroup->generateFilename(x, y);
      if (ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
	{
	  mTerrainGroup->defineTerrain(x, y);
	}
      else
	{
	  Image img;
	  getTerrainImage(x % 2 != 0, y % 2 != 0, img);
	  mTerrainGroup->defineTerrain(x, y, &img);
	  mTerrainsImported = true;
	}

    }
}

void ::Terrain::getTerrainImage(bool flipX, bool flipY, Image& img)
{
  img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  if (flipX)
    img.flipAroundY();
  if (flipY)
    img.flipAroundX();
}

void ::Terrain::configureTerrainDefaults(Light* l, SceneManager *mgr)
{
  // Configure global
  mTerrainGlobals->setMaxPixelError(16);
  // testing composite map
  mTerrainGlobals->setCompositeMapDistance(3000);

  // Important to set these so that the terrain knows what to use for derived (non-realtime) data
  mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
  mTerrainGlobals->setCompositeMapAmbient(mgr->getAmbientLight());
  mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

  // Configure default import settings for if we use imported image
  Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
  defaultimp.terrainSize = TERRAIN_SIZE;
  defaultimp.worldSize = TERRAIN_WORLD_SIZE;
  defaultimp.inputScale = heightScale;
  defaultimp.minBatchSize = 33;
  defaultimp.maxBatchSize = 65;

  // textures
  defaultimp.layerList.resize(1);
  defaultimp.layerList[0].worldSize = 100;
  defaultimp.layerList[0].textureNames.push_back("chess.png");
  defaultimp.layerList[0].textureNames.push_back("wierdterrain-normal.png");
}

void ::Terrain::setupContent(SceneManager *mgr)
{
  bool blankTerrain = false;
  //blankTerrain = true;

  mTerrainGlobals = new TerrainGlobalOptions();

  TerrainMaterialGeneratorPtr terrainMaterialGenerator = mTerrainGlobals->getDefaultMaterialGenerator();
  auto profile = dynamic_cast<TerrainMaterialGeneratorA::SM2Profile*>(terrainMaterialGenerator->getActiveProfile());
  profile->setLayerNormalMappingEnabled(true);
  profile->setLayerParallaxMappingEnabled(false);
  profile->setLayerSpecularMappingEnabled(false);
  profile->setGlobalColourMapEnabled(false);
  profile->setCompositeMapEnabled(false);
  profile->setReceiveDynamicShadowsEnabled(false);
  
  MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
  MaterialManager::getSingleton().setDefaultAnisotropy(7);

  LogManager::getSingleton().setLogDetail(LL_BOREME);

  mTerrainGroup = new TerrainGroup(mgr, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
  mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
  mTerrainGroup->setOrigin(Vector3::ZERO);

  Light *l = mgr->getLight("tstLight");
  configureTerrainDefaults(l, mgr);

  for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
    for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
      defineTerrain(x, y, blankTerrain);
  // sync load since we want everything in place when we start
  mTerrainGroup->loadAllTerrains(true);

  mTerrainGroup->freeTemporaryResources();
}

Ogre::Real ::Terrain::getHeight(Ogre::Real x, Ogre::Real y)
{
  return mTerrainGroup->getHeightAtWorldPosition(x, 100000.0, y);
}
