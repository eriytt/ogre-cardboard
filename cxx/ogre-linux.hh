#include "ogre.hh"

class OgreCardboardApp: public OgreApp
{
private:
  bool initialized = false;

protected:
  Ogre::Root* root = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::Camera* lcam = nullptr;
  Ogre::Camera* rcam = nullptr;
  Ogre::Viewport* lviewport = nullptr;
  Ogre::Viewport* rviewport = nullptr;

private:
  Ogre::RenderWindow* renderWindow = nullptr;

public:
  OgreCardboardApp();
  void initialize();
  void renderFrame();
  virtual void setupResources(Ogre::ResourceGroupManager &rgm) = 0;
  virtual void setupCamera() = 0; // Should set clip distances


  template <typename R>
  std::pair<R, R> forBothCameras(std::function<R (Ogre::Camera *camera)> f)
  { return std::pair<R, R>(f(lcam), f(rcam));}
  void forBothCameras(std::function<void (Ogre::Camera *camera)> f)
  { f(lcam); f(rcam);}


   template <typename R>
   std::pair<R, R> forBothCamerasAndViewports(std::function<R (Ogre::Camera *camera,
                                                               Ogre::Viewport *viewport)> f)
   { return std::pair<R, R>(f(lcam, lviewport), f(rcam, rviewport)); }

  void forBothCamerasAndViewports(std::function<void (Ogre::Camera *camera,
                                                      Ogre::Viewport *viewport)> f)
  { f(lcam, lviewport); f(rcam, rviewport); }



  size_t getWindowHandle();
};
