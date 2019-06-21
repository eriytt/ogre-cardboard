#include "vr/gvr/capi/include/gvr.h"

#include <android/log.h>
#include <android/asset_manager.h>

#define LOG_TAG "OgreClient Native"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#include "ogre.hh"

class OgreCardboardApp: public OgreApp
{
protected:
  static gvr::Rectf GVRFOV2FrustumExtents(gvr::Rectf& fov, float z_near);

private:
  bool initialized = false;
  const uint64_t kPredictionTimeWithoutVsyncNanos = 50000000;
  std::unique_ptr<gvr::GvrApi> gvr_api;
  std::unique_ptr<gvr::BufferViewportList> viewport_list;
  std::unique_ptr<gvr::SwapChain> swapchain;

protected:
  JNIEnv *env;
  jobject androidSurface;
  gvr_context *gvr;
  AAssetManager* amgr;

  Ogre::Root* root = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::Camera* lcam = nullptr;
  Ogre::Camera* rcam = nullptr;
  Ogre::Viewport* lviewport = nullptr;
  Ogre::Viewport* rviewport = nullptr;
  gvr::Rectf lFOV;
  gvr::Rectf rFOV;


private:
  Ogre::RenderWindow* renderWindow = nullptr;

public:
  OgreCardboardApp(JNIEnv *env, jobject androidSurface, gvr_context *gvr, AAssetManager* amgr);
  virtual void initialize();
  virtual void renderFrame();
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

};
