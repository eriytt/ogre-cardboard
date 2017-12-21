#if defined(ANDROID)
#  include "ogre-android.hh"
#else
#  include "ogre-linux.hh"
#endif

class OgreCardboardTestApp: public OgreCardboardApp
{
private:
  const char *VertexShader =
    "varying vec4 v_Color;\n"
    "uniform mat4 u_MVP;\n"
    "attribute vec4 vertex;\n"
    // "attribute vec4 foobar;\n"
    
    "void main()\n"
    "{\n"
    "	v_Color = vec4(0.0, 1.0, 0.5, 1.0) + vertex;\n"
    "   gl_Position = u_MVP * vertex + vec4(0.1, 0.1, 0.0, 0.0);\n"
    "}\n";

  const char *FragmentShader =
    "varying vec4 v_Color;\n"
    
    "void main()\n"
    "{\n"
    "	gl_FragColor = v_Color;\n"
    "}\n";

  const char *ShaderMaterialScript =
    "material myshadermaterial\n"
    "{\n"
    "    // first, preferred technique\n"
    "    technique\n"
    "    {\n"
    "        // first pass\n"
    "        pass\n"
    "        {\n"
    "            vertex_program_ref vTest\n"
    "            {\n"
    "                param_named_auto u_MVP worldviewproj_matrix\n"
    "            }\n"
    "            fragment_program_ref fTest\n"
    "            {\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "}\n";

  Ogre::SceneNode *mNode = nullptr;
  class Terrain *terrain = nullptr;
  bool forward = false, backward = false, left = false, right = false;
  
protected:
  void setupCamera();
  void setupResources(Ogre::ResourceGroupManager &rgm);

public:
#if defined(ANDROID)
  OgreCardboardTestApp(JNIEnv *env, jobject androidSurface, gvr_context *gvr, AAssetManager* amgr)
    : OgreCardboardApp(env, androidSurface, gvr, amgr) {}
#else
    OgreCardboardTestApp() : OgreCardboardApp() {}
#endif
  void initialize();
  void mainLoop();
  void handleKeyDown(int key);
  void handleKeyUp(int key);
};
