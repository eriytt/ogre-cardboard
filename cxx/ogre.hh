#include <stdexcept>

#include <Ogre.h>

class OgreApp
{

public:
  class OgreAppError : public std::runtime_error
  {
  public:
    OgreAppError(const std::string& what_arg): std::runtime_error(what_arg) {}
    OgreAppError(const char* what_arg): std::runtime_error(what_arg) {}
  };

public:
  OgreApp() {}
  virtual void initialize() = 0;
  virtual void renderFrame() = 0;
  virtual void setupResources(Ogre::ResourceGroupManager &rgm) = 0;
  virtual void setupCamera() = 0; // Should set clip distances
};
