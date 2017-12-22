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
$ $ ls lib/
libOgreMainStatic.a    libOgreRTShaderSystemStatic.a  libOgreVolumeStatic.a                 libPlugin_ParticleFXStatic.a
libOgrePagingStatic.a  libOgreTerrainStatic.a         libPlugin_OctreeSceneManagerStatic.a  libRenderSystem_GLES2Static.a
```
