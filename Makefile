
ANDROID_SDK = /home/tower/src/3rdparty/SDK
ANDROID_NDK = /home/tower/src/3rdparty/android-ndk-r13b
GVRSDK = $(ANDROID_NDK)/../gvr-android-sdk
GVRNDK = $(GVRSDK)/ndk

export ANDROID_SDK ANDROID_NDK GVRSDK GVRNDK

ADB = $(ANDROID_SDK)/platform-tools/adb
APK = project/bin/OgreClient-debug.apk

all:
	$(MAKE) -C cxx
	$(MAKE) -C project

$(APK):
	$(MAKE) -C project

install: $(APK)
	$(ADB) install -r $(APK)

run:
	$(ADB) shell am start -n com.towersmatrix.ogre/.OgreActivity

clean:
	$(MAKE) -C project clean
	$(MAKE) -C cxx clean

.PHONY: clean
