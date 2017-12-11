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
  // if a file is available, use it
  // if not, generate file from import

  // Usually in a real project you'll know whether the compact terrain data is
  // available or not; I'm doing it this way to save distribution size

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

void ::Terrain::initBlendMaps(Ogre::Terrain* terrain)
{
  TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
  TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
  Real minHeight0 = 70;
  Real fadeDist0 = 40;
  Real minHeight1 = 70;
  Real fadeDist1 = 15;
  float* pBlend1 = blendMap1->getBlendPointer();
  for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
      for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
	{
	  Real tx, ty;

	  blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
	  Real height = terrain->getHeightAtTerrainPosition(tx, ty);
	  Real val = (height - minHeight0) / fadeDist0;
	  val = Math::Clamp(val, (Real)0, (Real)1);
	  //*pBlend0++ = val;

	  val = (height - minHeight1) / fadeDist1;
	  val = Math::Clamp(val, (Real)0, (Real)1);
	  *pBlend1++ = val;


	}
    }
  blendMap0->dirty();
  blendMap1->dirty();
  //blendMap0->loadImage("blendmap1.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  blendMap0->update();
  blendMap1->update();

  // set up a colour map
  /*
    if (!terrain->getGlobalColourMapEnabled())
    {
    terrain->setGlobalColourMapEnabled(true);
    Image colourMap;
    colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    terrain->getGlobalColourMap()->loadImage(colourMap);
    }
  */

}

void ::Terrain::configureTerrainDefaults(Light* l, SceneManager *mgr)
{
  // Configure global
  mTerrainGlobals->setMaxPixelError(8);
  // testing composite map
  mTerrainGlobals->setCompositeMapDistance(3000);

  // Important to set these so that the terrain knows what to use for derived (non-realtime) data
  mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
  mTerrainGlobals->setCompositeMapAmbient(mgr->getAmbientLight());
  mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());
  //mTerrainGlobals->setVisibilityFlags(0);
  //mTerrainGlobals->setCompositeMapAmbient(ColourValue(1.0, 0.0, 0.0));
  //mTerrainGlobals->setCompositeMapDiffuse(ColourValue(0.0, 1.0, 0.0));

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
  // defaultimp.layerList[1].worldSize = 30;
  // defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
  // defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
  // defaultimp.layerList[2].worldSize = 200;
  // defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
  // defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void ::Terrain::setupContent(SceneManager *mgr)
{
  bool blankTerrain = false;
  //blankTerrain = true;



  mTerrainGlobals = new TerrainGlobalOptions();

  // We can use the default (SM2) material generator on android
  // TerrainMaterialGeneratorPtr terrainMaterialGenerator;
  // TerrainMaterial *terrainMaterial = new TerrainMaterial(resourceGroup);
  // terrainMaterialGenerator.bind(terrainMaterial);
  // mTerrainGlobals->setDefaultMaterialGenerator(terrainMaterialGenerator);

  TerrainMaterialGeneratorPtr terrainMaterialGenerator = mTerrainGlobals->getDefaultMaterialGenerator();
  auto profile = dynamic_cast<TerrainMaterialGeneratorA::SM2Profile*>(terrainMaterialGenerator->getActiveProfile());
  profile->setLayerNormalMappingEnabled(true);
  profile->setLayerParallaxMappingEnabled(false);
  profile->setLayerSpecularMappingEnabled(false);
  profile->setGlobalColourMapEnabled(false);
  profile->setCompositeMapEnabled(false);
  profile->setReceiveDynamicShadowsEnabled(false);
  

  // Set Ogre Material with the name "TerrainMaterial" in constructor


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

  // if (mTerrainsImported)
  //   {
  //     TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
  //     while(ti.hasMoreElements())
  //       {
  //         Ogre::Terrain* t = ti.getNext()->instance;
  //         initBlendMaps(t);
  //       }
  //   }

  mTerrainGroup->freeTemporaryResources();
}

Ogre::Real ::Terrain::getHeight(Ogre::Real x, Ogre::Real y)
{
  return mTerrainGroup->getHeightAtWorldPosition(x, 100000.0, y);
}
