OGREDEPS =        /home/tower/src/3rdparty/AndroidDependencies
# OGRE_PATH =       /home/tower/src/3rdparty/ogre-git/ogre
# OGRE_BUILD_PATH = /home/tower/src/3rdparty/ogre-git/ogre-build-android
OGRE_PATH =       /home/tower/src/3rdparty/ogre
OGRE_BUILD_PATH = /home/tower/src/3rdparty/ogre-build-android

ANDROID_TOOLCHAIN_NAME = arm-linux-androideabi
ANDROID_TOOLCHAIN_VERSION = 4.9
ANDROID_NDK_HOST_SYSTEM_NAME = linux-x86_64
ANDROID_ABI = armeabi-v7a
ANDROID_NATIVE_API_LEVEL=23
ANDROID_ARCH_NAME = arm

ANDROID_TOOLCHAIN_ROOT = $(ANDROID_NDK)/toolchains/$(ANDROID_TOOLCHAIN_NAME)-$(ANDROID_TOOLCHAIN_VERSION)/prebuilt/${ANDROID_NDK_HOST_SYSTEM_NAME}
ANDROID_SYSROOT = $(ANDROID_NDK)/platforms/android-$(ANDROID_NATIVE_API_LEVEL)/arch-$(ANDROID_ARCH_NAME)

CC = $(ANDROID_TOOLCHAIN_ROOT)/bin/$(ANDROID_TOOLCHAIN_NAME)-gcc
CXX = $(ANDROID_TOOLCHAIN_ROOT)/bin/$(ANDROID_TOOLCHAIN_NAME)-g++
LD = $(ANDROID_TOOLCHAIN_ROOT)/bin/$(ANDROID_TOOLCHAIN_NAME)-ld

OBJECTS = jni.o testapp.o ogre.o Terrain.o
DEBUG_CXXFLAGS = #-g
OPT_CXXFLAGS = -O2
SYSTEM_CXXFLAGS = --sysroot=${ANDROID_SYSROOT} \
-isystem $(ANDROID_SYSROOT)/usr/include \
-isystem $(ANDROID_NDK)/sources/android/cpufeatures \
-isystem $(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_TOOLCHAIN_VERSION)/include \
-isystem $(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_TOOLCHAIN_VERSION)/libs/$(ANDROID_ABI)/include
OGRE_CXXFLAGS = -I$(OGRE_PATH)/OgreMain/include -I$(OGRE_PATH)/RenderSystems/GLES2/include -I$(OGRE_PATH)/Components/Terrain/include -I$(OGRE_BUILD_PATH)/include
GVR_CXXFLAGS = -I$(GVRNDK)/include

CXXFLAGS = $(DEBUG_CXXFLAGS) $(OPT_CXXFLAGS) -std=c++14 -Wall -fPIC -frtti -fexceptions $(SYSTEM_CXXFLAGS) $(OGRE_CXXFLAGS) $(GVR_CXXFLAGS)


SYSTEM_LDFLAGS = --sysroot=${ANDROID_SYSROOT} \
-L$(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_TOOLCHAIN_VERSION)/libs/$(ANDROID_ABI) \
-lgnustl_static -llog -lEGL -landroid
GVR_LDFLAGS = -L$(GVRNDK)/lib/android_$(ANDROID_ARCH_NAME) -lgvr
OGREDEPS_LDFLAGS = -L$(OGREDEPS)/lib/$(ANDROID_ABI) -lFreeImage -lzzip
OGRE_LDFLAGS = -L$(OGRE_BUILD_PATH)/lib -lRenderSystem_GLES2Static -lOgreGLSupportStatic -lOgreMainStatic -L$(OGRE_BUILD_PATH)/OgreJNI/obj/local/$(ANDROID_ABI) -lcpufeatures -lGLESv2

OGREDEPS_LDFLAGS = -L$(OGREDEPS)/lib/$(ANDROID_ABI) -lFreeImage -lzzip -lzlib
OGRE_LDFLAGS = -L$(OGRE_BUILD_PATH)/lib -lRenderSystem_GLES2Static -lOgreTerrainStatic -lOgreMainStatic -L$(OGRE_BUILD_PATH)/OgreJNI/obj/local/$(ANDROID_ABI) -lcpufeatures -lGLESv2 $(OGREDEPS_LDFLAGS)

TEST_LDFLAGS = -lGLESv2
LDFLAGS = -shared -fPIC $(OGRE_LDFLAGS) $(GVR_LDFLAGS) $(SYSTEM_LDFLAGS) \
-Wl,-uJava_com_towersmatrix_ogre_OgreActivity_nativeInitOgre

ogreclient.so: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) ogreclient.so
