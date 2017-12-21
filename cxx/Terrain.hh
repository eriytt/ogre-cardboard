#ifndef TERRAIN_H
#define TERRAIN_H

#include <Ogre.h>
#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>

class Terrain
{
 protected:
  Ogre::TerrainGlobalOptions* mTerrainGlobals;
  Ogre::TerrainGroup* mTerrainGroup;
  bool mTerrainsImported;
  Ogre::Real heightScale;
  const Ogre::String resourceGroup;

public:
  Terrain(Ogre::Root *root, const Ogre::String &resourceGroupName);
  virtual ~Terrain();
  Ogre::AxisAlignedBox getBounds() {return  mTerrainGroup->getTerrain(0, 0)->getWorldAABB();}
  Ogre::Real getHeight(Ogre::Real x, Ogre::Real y);
  Ogre::TerrainGroup *getTerrainGroup() {return mTerrainGroup;}

protected:
  void defineTerrain(long x, long y, bool flat = false);
  void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);
  void initBlendMaps(Ogre::Terrain* terrain);
  void configureTerrainDefaults(Ogre::Light* l, Ogre::SceneManager *mgr);
  void setupContent(Ogre::SceneManager *mgr);
  void saveTerrains(bool onlyIfModified);
};

#define TERRAIN_WORLD_SIZE 2048.0f
#define TERRAIN_SIZE 513
#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

#endif // TERRAIN_H
