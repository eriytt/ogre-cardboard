# ogre-cardboard
Ogre VR for Cardboard capable Android devices

## Building
### Prerequisites
* Android SDK
* Android NDK
* GoogleVR SDK
* AndroidDependencies for Ogre3D
* Ogre3D

#### Installing Android Stuff
I installed this a long time ago and I don't remember the exact steps, but as far as I can rememeber, this is what I did:

##### Download and install [Android Studio](https://developer.android.com/studio/index.html).
Android Studio comes bundled with the Android SDK. I know downloading and installing it is a total waste, but nowadays it seems like the only way to get hold of it. The good news is that once the Android SDK is installed, you can just get rid of Android Studio, there's no reason to keep it.

##### Install the required packages
Start the SDK manager in 
```
$ <SDK>/tools/android
```
I think these are the important ones
* Android SDK Tools (25.1.7)
* Android SDK Platform-tools (23.1)
* Android SDK Build-tools (I have version 23.0.3 and 24, but I think only the latter is needed)

This step can probably be achieved through Android Studio as well if one wants to go through that hassle.

##### Install the [Android NDK](https://developer.android.com/ndk/downloads/older_releases.html)
You want version **r13b**, other will likely not work since Google is well known to introduce incompatible changes for every new release. If you want to make life easy, you put the NDK in the same directory as the SDK.

##### Install the [GoogleVR SDK](https://github.com/googlevr/gvr-android-sdk)
Finally a simple one, just git clone it. Since this one is even less stable then the NDK, you need to go back to version 1.0.3 which has SHA1 ID 3360cbd21d8ba07c305fa379b905ff8c7632f6cc:
```
$ git checkout 3360cbd21d8ba07c305fa379b905ff8c7632f6cc
```
Again, cloning it to the same directory that you put the SDK in makes life somewhat easier.

##### Get the Ogre3D [AndroidDependencies](https://sourceforge.net/projects/ogre/files/ogre-dependencies-android/1.9/AndroidDependencies_17_12_2012.zip/download)
I have it unzipped in the same directory as all the previous stuff.

##### Clone Ogre3D 1.9
We need to build Ogre3D from source, moreover, we need version 1.9, so we need to clone the official Mercurial repo:
```
$ hg clone https://bitbucket.org/sinbad/ogre
```

Because we need a specific version we need to go back to changeset 10420:46b36c014565.
```
$ hg up 10420:46b36c014565
```

I have made some necessary changes to this revision, [details here](https://github.com/eriytt/ogre-cardboard/commit/dbdc85463bb897ebb72e56db00dd2e2a43687bc4), so we need to apply the patch included in this project:
```
$ patch -p 1 < <ogre-cardboard>/ogre.patch
```

Building Ogre3D is done with cmake and I build it in another directory than the actual source. I use this curse (in a script called ogre-build-android.sh) to build it:
```
#!/bin/bash

WORKSPACE=/home/tower/src/3rdparty
OGRE=$WORKSPACE/ogre
BUILD=$WORKSPACE/ogre-build-android
OGREDEPS=$WORKSPACE/AndroidDependencies

ANDROID_NDK=$WORKSPACE/android-ndk-r13b
ANDROID_SDK=$WORKSPACE/SDK

ZZIP_HOME=$OGREDEPS
OGRE_DEPENDENCIES_DIR=$OGREDEPS

export ANDROID_NDK ANDROID_SDK ZZIP_HOME OGREDEPS

ZZIP_FLAGS="-DZZip_INCLUDE_DIRS=$OGREDEPS/include -DZZip_LIBRARIES=$OGREDEPS/lib/armeabi-v7a/libzzip.a -DZZip_FOUND=1"
FREEIMAGE_FLAGS="-DFreeImage_INCLUDE_DIRS=$OGREDEPS/include -DFreeImage_LIBRARIES=$OGREDEPS/lib/armeabi-v7a/libFreeImage.a -DFreeImage_FOUND=1"

cmake -DCMAKE_TOOLCHAIN_FILE=$OGRE/CMake/toolchain/android.toolchain.cmake -DOGRE_DEPENDENCIES_DIR=$OGREDEPS -DANDROID_ABI=armeabi-v7a  -DANDROID_NATIVE_API_LEVEL=23 -DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.9 -DFREETYPE_FT2BUILD_INCLUDE_DIR=$OGREDEPS/include -DOIS_INCLUDE_DIR=$OGREDEPS/include -DEXTRA_CXX_FLAGS=-funwind-tables -DEXTRA_C_FLAGS=-funwind-tables $ZZIP_FLAGS $FREEIMAGE_FLAGS -DOGRE_CONFIG_ENABLE_ZIP=1 $OGRE

```

You need to change the path at the top. Instead of _WORKSPACE=/home/tower/src/3rdparty_ it should say something like _WORKSPACE=/home/you/the/directory/where/you/put/all/the/stuff_. Then do something like
```
$ mkdir ogre-build-android && cd ogre-build-android
$ ../ogre-build-android.sh
```
You may have to run the actual script twice, I have not figured out why yet.
After that, just:
```
$ make -j5
```
and sit back and relax for a while.

After a while you should se a build failure trying to build the Android JNI sample application. Ironically, this is fine, I haven't bothered to fix this because it's not needed, the libraries have been built already:
```
$ ls lib/
libOgreMainStatic.a    libOgreRTShaderSystemStatic.a  libOgreVolumeStatic.a                 libPlugin_ParticleFXStatic.a
libOgrePagingStatic.a  libOgreTerrainStatic.a         libPlugin_OctreeSceneManagerStatic.a  libRenderSystem_GLES2Static.a
```

### Ogre Cardboard

You need to edit the top level Makefile and set the variables ANDROID_SDK and ANDROID_NDK, and cxx/Makefile OGREDEPS, OGRE_PATH and OGRE_BUILD_PATH. If you followed my advice on how to place prerequisites, this is everything hat needs to be done before running:
```
$ make
```
If you didn't, there are some other variables you need to change as well. Anyway, after having run make successfully, you should have a project/bin/OgreClient-debug.apk. Connect your device and run
```
$ make install
```
then turn your device on and run
```
$ make run
```
If everything works as it should, you should see the familiar cardboard view soaring of a red/white checker landscape with some small floating cubes here and there, and a mysteriously spinning rod.
If you have a Bluetooth keyboard, you should be able to move around some with the keys w, s, a and d.

### Building a native application
With some tweaks, notably switching render system to a native render system and defining other resource locations, the application can be run natively on the development system. Debian packages Ogre and its dependencies so all you need to do is install them. I have included native support in the cxx/Makefile.linux makefile. Build it with:
```
$ cd cxx && make
```
and run it with:
```
$ ./ogreclient
```
standing in the cxx directory.

## Implementation
### High level view
Like any other app with native code, Ogre Cardboard has some Java boilerplate that sets up the Activity and View and creates a Renderer, where the Renderer calls native code through JNI to initialize the Cardboard and Ogre state. After everything is set, the renderer's onDrawFrame() method is called (periodically? when idle? as fast as possible?) which call the native main loop through JNI.

### Setup
A lot of the setup is taken directly from the Google VR samples. The trick is "fake" an Ogre state, and while setting this state up, don't mess with the state that Google VR already set up. This is done by setting flags to use an already existing EGL Context. Since all Google VR seem to do is setting up a special frame buffer object to render to, and viewports projection matrices for each eye, mimicing this setup in Ogre is fairly easy. The tricky part was getting Ogre to render to the GVR frame buffer object, since Ogre has no idea that this object even exist, and there was no API to inform Ogre about it. Now there is...

#### Window Size
One particular thing that I did not implement was the ability to select window (frame buffer object) size. The size is going to be whatever eglQuerySurface() returns.

### Main Loop
The main loop applies the head rotation matrix to the view matrix for both cameras. This is the last thing that happens, and Ogre::Camera::setCustomViewMatrix() is used to prevent the view matrix from being recalculated automatically by Ogre somewhere in the rendering code. This means that after all rendering is done, Ogre::Camera::setCustomViewMatrix(false) is called to reenable automatic updates of the view matrix when setting camera positions and such.
Before doing the actual rendering, the default framebuffer object is set in Ogre, otherwise Ogre would render directly to the object that corresponds to the actual window displayed on the device screen.
What follows is a slightly customized render sequence that makes sure that a call to swapping buffers is _not_ made. Otherwise it very similar to Ogre::Root::renderOneFrame(). The actual swapping of something to the visible screen is done by gvr::Frame::Submit().
The main loop is not actually a loop but a function that should be called periodically.

### Resources
Most stuff in Ogre is not actual code, but rather resources like meshes, material definitions and shader programs loaded from files. Ogre has built in support for loading resources from somewhere in the APK file that is actually running. All that needs to be done is defining the locations, and this is done in OgreCardboardTestApp::setupResources() and then drop in the stuff you need under project/assets. Note that resource locations are not recursive by default.

### Known Issues
Apart from the window size issue described above, I've had problems with holes in the terrain (actually, most of the terrain is not drawn to screen). The missing terrain patches seem to occur when no other object is inside the view frustum. I have verified that this is a problem that does not occur on all devices, so I'm suspecting a driver bug. But it could just as well be some undefined behavior that I have introduced in my shader code. If you figure out what the problem is, please let me know.

If an object is not explicitly assigned a material, Ogre will assign it a default material, either BaseWhite or BaseWhiteNoLighting. These materials do not have shader techniques defined, and since OpenGLES 2.0 is a shader only render system, these objects will not be visible. You must use materials that has a shader based technique. I've seen snippets that augment the default materials with such techniques, but I have not made it in this sample, instead i have created my own "myshadermaterial". This material does not work very well running on a native linux host with the OpenGL render system though, but at least it's visible.
