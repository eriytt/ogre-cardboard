#include "TerrainMaterial.hh"

//#include "Utils.h"

TerrainMaterial::TerrainMaterial(const Ogre::String &resourceGroupName)
{
  mProfiles.push_back(OGRE_NEW Profile(this, "SimpleMaterial", "Profile for rendering shaded terrain material", resourceGroupName));
    setActiveProfile("SimpleMaterial");
}


TerrainMaterial::Profile::Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc,
				  const Ogre::String& resourceGroupName)
  : Ogre::TerrainMaterialGenerator::Profile(parent, name, desc), resourceGroup(resourceGroupName)
{
}

TerrainMaterial::Profile::~Profile()
{
}

Ogre::MaterialPtr TerrainMaterial::Profile::generate(const Ogre::Terrain* terrain)
{
  //TerrainMaterial* parent = (TerrainMaterial*)getParent();
  const Ogre::String shadow_name("shadow");

  // Set Ogre material
  Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("shaded_terrain", resourceGroup, false);
  Ogre::Technique *tech = mat->getTechnique(0);

  Ogre::Pass *pass = tech->getPass(0);
  pass->setLightingEnabled(false);
  pass->setSceneBlending(Ogre::SBT_REPLACE);
  pass->setDepthWriteEnabled(true);
  pass->removeAllTextureUnitStates();

  Ogre::uint32 size = terrain->getSize() - 1;
  Ogre::Real grid_size = terrain->getWorldSize() / static_cast<Ogre::Real>(terrain->getSize() - 1);
  Ogre::TexturePtr shadow_tex = Ogre::TextureManager::getSingleton().createManual(shadow_name, resourceGroup, Ogre::TEX_TYPE_2D, size, size, /*num_mips*/ 1, Ogre::PF_X8R8G8B8);
  Ogre::HardwarePixelBufferSharedPtr buf = shadow_tex->getBuffer();

  size_t real_size = buf->getSizeInBytes();
  unsigned char *data = static_cast<unsigned char*>(buf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

  for (Ogre::uint32 x = 0; x < size; ++x)
    for (Ogre::uint32 y = 0; y < size; ++y)
    {
      Ogre::Vector3 q0(      0.0,       0.0, terrain->getHeightAtPoint(x    , size - y    ));
      Ogre::Vector3 q1(grid_size,       0.0, terrain->getHeightAtPoint(x + 1, size - y    ));
      Ogre::Vector3 q2(      0.0, grid_size, terrain->getHeightAtPoint(x    , size - y + 1));
      Ogre::Vector3 q3(grid_size, grid_size, terrain->getHeightAtPoint(x + 1, size - y + 1));

      Ogre::Vector3 n0((q2 - q0).crossProduct(q1 - q0).normalisedCopy());
      Ogre::Vector3 n1((q1 - q3).crossProduct(q2 - q3).normalisedCopy());
      Ogre::Vector3 n((n0 + n1).normalisedCopy());

      Ogre::Real intensity = n.dotProduct(Ogre::Vector3(0.0, 0.0, -1.0));

      Ogre::uint32 tex_idx = ((y * size) + x) * 4;

      assert(tex_idx + 3 < real_size);
      data[tex_idx + 0] = 0;
      data[tex_idx + 1] = static_cast<unsigned char>(intensity * 255);
      data[tex_idx + 2] = 0;
      data[tex_idx + 3] = 255;
    }
  buf->unlock();


  Ogre::TextureUnitState *tex = pass->createTextureUnitState();
  tex->setTextureName(shadow_tex->getName());

  Ogre::TextureUnitState *tex2 = pass->createTextureUnitState("terrain_detail.jpg");
  tex2->setColourOperation(Ogre::LBO_MODULATE);
  tex2->setTextureScale(0.1, 0.1);

  Ogre::TextureUnitState *tex3 = pass->createTextureUnitState("terrain_detail.jpg");
  tex3->setColourOperation(Ogre::LBO_MODULATE);
  tex3->setTextureScale(0.01, 0.01);

  return mat;
}

Ogre::MaterialPtr TerrainMaterial::Profile::generateForCompositeMap(const Ogre::Terrain* terrain)
{
  return terrain->_getCompositeMapMaterial();
}

Ogre::uint8 TerrainMaterial::Profile::getMaxLayers(const Ogre::Terrain* terrain) const
{
    return 0;
}

void TerrainMaterial::Profile::updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
{
}

void TerrainMaterial::Profile::updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
{
}

void TerrainMaterial::Profile::requestOptions(Ogre::Terrain* terrain)
{
    terrain->_setMorphRequired(false);
    terrain->_setNormalMapRequired(false);
    terrain->_setLightMapRequired(false);
    terrain->_setCompositeMapRequired(false);
}

void TerrainMaterial::Profile::setLightmapEnabled(bool enabled)
{
}
