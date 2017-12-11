#include "vr/gvr/capi/include/gvr.h"

#include <android/log.h>
#include <android/asset_manager.h>

#include <Ogre.h>

#define LOG_TAG "OgreClient Native"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// namespace OgreClient {
//   void initOgre(JNIEnv *env, jobject surface, gvr_context *gvr,  AAssetManager* amgr);
//   void render();
//   void handleKeyDown(int key);
//   void handleKeyUp(int key);
// }

class OgreCardboardApp
{

public:
  class OgreCardboardAppError : public std::runtime_error
  {
  public:
    OgreCardboardAppError(const std::string& what_arg): std::runtime_error(what_arg) {}
    OgreCardboardAppError(const char* what_arg): std::runtime_error(what_arg) {}
  };

private:
  static Ogre::Matrix4 PerspectiveMatrixFromView(const gvr::Rectf& fov, float z_near, float z_far);
  
  
private:
  bool initialized = false;
  const uint64_t kPredictionTimeWithoutVsyncNanos = 50000000;
  std::unique_ptr<gvr::GvrApi> gvr_api;
  std::unique_ptr<gvr::BufferViewportList> viewport_list;
  std::unique_ptr<gvr::SwapChain> swapchain;

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
  virtual void initialize(JNIEnv *env, jobject androidSurface, gvr_context *gvr, AAssetManager* amgr);
  void renderFrame();
  virtual void setupResources(Ogre::ResourceGroupManager &rgm) = 0;
  virtual void setupCamera() = 0; // Should set clip distances

  template <typename R>
  std::pair<R, R> forBothCameras(std::function<R (Ogre::Camera *camera)> f)
  { return std::pair<R, R>(f(lcam), f(rcam));}
  void forBothCameras(std::function<void (Ogre::Camera *camera)> f)
  { f(lcam); f(rcam);}

  
  // template <typename R>
  // std::pair<R, R> forBothViewports(R f(Ogre::Viewport *viewport)) { return std::pair<R, R>(f(lviewport), f(rviewport));}
   template <typename R>
   std::pair<R, R> forBothCamerasAndViewports(std::function<R (Ogre::Camera *camera, Ogre::Viewport *viewport)> f)
   { return std::pair<R, R>(f(lcam, lviewport), f(rcam, rviewport)); }

  void forBothCamerasAndViewports(std::function<void (Ogre::Camera *camera, Ogre::Viewport *viewport)> f)
  { f(lcam, lviewport); f(rcam, rviewport); }

};
