#include <sys/prctl.h>

#include <android/asset_manager_jni.h>

#include <jni.h>
//#include "vr/gvr/capi/include/gvr.h"

#include "testapp.hh"

#define JNI_METHOD(return_type, method_name)                    \
  JNIEXPORT return_type JNICALL                                 \
  Java_com_towersmatrix_ogre_Native_##method_name

static OgreCardboardTestApp *app;

extern "C" {
 
JNI_METHOD(jlong, InitOgre)(JNIEnv *env, jclass clazz,
                            jobject surface, jlong native_gvr_api, jobject assetManager)
{
  prctl(PR_SET_DUMPABLE, 1);
  AAssetManager* assetMgr = AAssetManager_fromJava(env, assetManager);
  app = new OgreCardboardTestApp(env, surface, reinterpret_cast<gvr_context *>(native_gvr_api), assetMgr);
  app->initialize();
  //OgreClient::initOgre(env, surface, reinterpret_cast<gvr_context *>(native_gvr_api), assetMgr);
  return 0;
}
  
JNI_METHOD(void, Render)(JNIEnv *env, jclass clazz)
{
  app->mainLoop();
  //OgreClient::render();
}

JNI_METHOD(void, HandleKeyDown)(JNIEnv *env, jclass clazz, jint key)
{
  app->handleKeyDown(static_cast<int>(key));
}

JNI_METHOD(void, HandleKeyUp)(JNIEnv *env, jclass clazz, jint key)
{
  app->handleKeyUp(static_cast<int>(key));
}

} // extern "C"
