#ifndef TERRAINMATERIAL_H
#define TERRAINMATERIAL_H

// V1.0

#include "Ogre.h"
#include "OgreTerrain.h"
#include "OgreTerrainMaterialGenerator.h"

class TerrainMaterial : public Ogre::TerrainMaterialGenerator
{
public:

  TerrainMaterial(const Ogre::String& resourceGroupName);

  class Profile : public Ogre::TerrainMaterialGenerator::Profile
  {
  protected:
    const Ogre::String &resourceGroup;

  public:
    Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc,
	    const Ogre::String& resourceGroupName);
    ~Profile();

    bool isVertexCompressionSupported() const { return false; }
    Ogre::MaterialPtr generate(const Ogre::Terrain* terrain);
    Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain* terrain);
    Ogre::uint8 getMaxLayers(const Ogre::Terrain* terrain) const;
    void updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);
    void updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);
    void requestOptions(Ogre::Terrain* terrain);
    void setLightmapEnabled(bool enabled);

  };
};

#endif

