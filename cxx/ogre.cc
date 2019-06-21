#include "ogre-android.hh"

//#include <string>
#include<sstream>
#include <unistd.h>

#include <android/native_window_jni.h>

#include <EGL/egl.h>

#include <GLES2/gl2.h>


#include <OgreGLES2Plugin.h>
#include <Android/OgreAPKFileSystemArchive.h>
#include <Android/OgreAPKZipArchive.h>

#define LOG_INFO LOGI
#define LOG_ERROR LOGE

OgreCardboardApp::OgreCardboardApp(JNIEnv *env, jobject androidSurface, gvr_context *gvr, AAssetManager* amgr): OgreApp(), env(env), androidSurface(androidSurface), gvr(gvr), amgr(amgr)
{
}

template <typename T>
std::string ToString(T val)
{
  std::stringstream stream;
  stream << val;
  return stream.str();
}

void OgreCardboardApp::initialize()
{
    gvr_api = gvr::GvrApi::WrapNonOwned(gvr);
    gvr_api->InitializeGl();

    EGLDisplay display;
    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
      throw OgreAppError(std::string("eglGetDisplay() returned error ")
                                  + ToString(eglGetError()));

    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    if (surface == EGL_NO_SURFACE)
      throw OgreAppError("eglGetCurrentSurface(EGL_DRAW) returned error returned EGL_NO_SURFACE");


    int width, height;
    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height))
      throw OgreAppError(std::string("eglQuerySurface() returned error ")
                                  + ToString(eglGetError()));

    LOG_INFO("EGl Surface size: %dx%d", width, height);

    LOG_INFO("Initializing: GetCurrentContext");
    EGLContext context  = eglGetCurrentContext();

    LOG_INFO("Initializing: MakeCurrent");
    if (!eglMakeCurrent(display, surface, surface, context)) {
      LOG_ERROR("eglMakeCurrent() returned error %d", eglGetError());
      return;
    }

    // TODO: Make Ogre use the render size that we pass in, instead of auto detecting the
    //       window size. After all, we are not rendering to the window, but to a backbuffer
    //       managed by GVR.
    //       Update: Ogre uses the size of the current EGL surface, but we are not drawing to
    //       a window, so we should force Ogre to use our size. It's not clear how to do that
    //       though...

    // const gvr::Sizei &max_size = gvr_api->GetMaximumEffectiveRenderTargetSize();

    // For multisampling level 2, only half of the pixels needed with similar quality
    // render_size = {.width = (7 * max_size.width) / 10,
    //                .height = (7 * max_size.height) / 10};

    gvr::Sizei render_size = {.width  = width,
                              .height = height};


    root = OGRE_NEW Ogre::Root();
    auto renderSystemPlugin = OGRE_NEW Ogre::GLES2Plugin();
    root->installPlugin(renderSystemPlugin);
    Ogre::RenderSystem *rs = root->getAvailableRenderers().at(0);
    root->setRenderSystem(rs);
    rs->setFixedPipelineEnabled(false); // TODO: is this needed or does it help in any way?

    root->initialise(false);

    Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKFileSystemArchiveFactory(amgr));
    Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKZipArchiveFactory(amgr));

    Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();
    // Call concrete implementation
    setupResources(rgm);
    rgm.initialiseAllResourceGroups();

    ANativeWindow* nativeWnd = ANativeWindow_fromSurface(env, androidSurface);
    Ogre::NameValuePairList opt;
    opt["externalWindowHandle"] = Ogre::StringConverter::toString((int)nativeWnd);
    opt["currentGLContext"]     = Ogre::StringConverter::toString(true);
    renderWindow = root->createRenderWindow("OgreWindow", render_size.width, render_size.height, false, &opt);

    sceneManager = root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");

    lcam = sceneManager->createCamera("LeftEye");
    rcam = sceneManager->createCamera("RightEye");

    // Call concrete implementation to set up cameras
    setupCamera();

    std::vector<gvr::BufferSpec> specs;
    specs.push_back(gvr_api->CreateBufferSpec());
    specs[0].SetSize(render_size);
    specs[0].SetColorFormat(GVR_COLOR_FORMAT_RGBA_8888);
    specs[0].SetDepthStencilFormat(GVR_DEPTH_STENCIL_FORMAT_DEPTH_16);
    specs[0].SetSamples(2);
    swapchain.reset(new gvr::SwapChain(gvr_api->CreateSwapChain(specs)));

    viewport_list.reset(new gvr::BufferViewportList(gvr_api->CreateEmptyBufferViewportList()));
    viewport_list->SetToRecommendedBufferViewports();


    // Left viewport
    gvr::BufferViewport tmp_viewport(gvr_api->CreateBufferViewport());
    viewport_list->GetBufferViewport(0, &tmp_viewport);
    lFOV = tmp_viewport.GetSourceFov();
    auto e = GVRFOV2FrustumExtents(lFOV, lcam->getNearClipDistance());
    lcam->setFrustumExtents(e.left, e.right, e.top, e.bottom);
    gvr::Mat4f left_eye_matrix = gvr_api->GetEyeFromHeadMatrix(GVR_LEFT_EYE);
    lcam->setFrustumOffset(left_eye_matrix.m[0][3], left_eye_matrix.m[1][3]);

    auto vpUV = tmp_viewport.GetSourceUv();
    lviewport = renderWindow->addViewport(lcam, 0,
                                          vpUV.left,
                                          1.0f - vpUV.top,
                                          vpUV.right - vpUV.left,
                                          vpUV.top - vpUV.bottom);

    // Right viewport
    viewport_list->GetBufferViewport(1, &tmp_viewport);
    rFOV = tmp_viewport.GetSourceFov();
    e = GVRFOV2FrustumExtents(rFOV, rcam->getNearClipDistance());
    rcam->setFrustumExtents(e.left, e.right, e.top, e.bottom);
    gvr::Mat4f right_eye_matrix = gvr_api->GetEyeFromHeadMatrix(GVR_RIGHT_EYE);
    rcam->setFrustumOffset(right_eye_matrix.m[0][3], right_eye_matrix.m[1][3]);


    vpUV = tmp_viewport.GetSourceUv();
    rviewport = renderWindow->addViewport(rcam, 1,
                                          vpUV.left,
                                          1.0f - vpUV.top,
                                          vpUV.right - vpUV.left,
                                          vpUV.top - vpUV.bottom);

    initialized = true;
}

gvr::Rectf OgreCardboardApp::GVRFOV2FrustumExtents(gvr::Rectf& fov, float z_near)
{
  return  {
    .left = static_cast<float>(-std::tan(fov.left * M_PI / 180.0f) * z_near),
    .right = static_cast<float>(std::tan(fov.right * M_PI / 180.0f) * z_near),
    .bottom = static_cast<float>(-std::tan(fov.bottom * M_PI / 180.0f) * z_near),
    .top = static_cast<float>(std::tan(fov.top * M_PI / 180.0f) * z_near)
  };
}

void OgreCardboardApp::renderFrame()
{
  if (not initialized)
    throw OgreAppError("OgreCardboardApp has not been initialized");

  gvr::ClockTimePoint target_time = gvr::GvrApi::GetTimePointNow();
  target_time.monotonic_system_time_nanos +=
    kPredictionTimeWithoutVsyncNanos;

    gvr::Mat4f head_view = gvr_api->GetHeadSpaceFromStartSpaceRotation(target_time);

    const Ogre::Matrix4 ogreHeadView
      (head_view.m[0][0], head_view.m[0][1], head_view.m[0][2], head_view.m[0][3],
       head_view.m[1][0], head_view.m[1][1], head_view.m[1][2], head_view.m[1][3],
       head_view.m[2][0], head_view.m[2][1], head_view.m[2][2], head_view.m[2][3],
       head_view.m[3][0], head_view.m[3][1], head_view.m[3][2], head_view.m[3][3]);

    forBothCameras([&ogreHeadView](Ogre::Camera *cam) {
        const Ogre::Matrix4 &view = cam->getViewMatrix();
        cam->setCustomViewMatrix(true, ogreHeadView * view);
      });

    gvr::Frame frame = swapchain->AcquireFrame();
    frame.BindBuffer(0);

    auto rs = root->getRenderSystem();
    static_cast<Ogre::GLES2RenderSystem*>(rs)->setDefaultFBO(frame.GetFramebufferObject(0));

    Ogre::WindowEventUtilities::messagePump(); // Should do nothing

    // Custom rendering
    if(!root->_fireFrameStarted())
      return;

    // update all targets but don't swap buffers
    rs->_updateAllRenderTargets(false);

    // give client app opportunity to use queued GPU time
    bool ret = root->_fireFrameRenderingQueued();

    // This belongs here, as all render targets must be updated before events are
    // triggered, otherwise targets could be mismatched.  This could produce artifacts,
    // for instance, with shadows.
    for (Ogre::SceneManagerEnumerator::SceneManagerIterator it = root->getSceneManagerIterator();
         it.hasMoreElements(); it.moveNext())
      it.peekNextValue()->_handleLodEvents();
    if (not ret)
      return;

    root->_fireFrameEnded();

    frame.Unbind();
    frame.Submit(*viewport_list, head_view);

    forBothCameras([](Ogre::Camera *cam) {
        cam->setCustomViewMatrix(false);
      });
}
