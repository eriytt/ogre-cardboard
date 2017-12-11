package com.towersmatrix.ogre;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.SurfaceHolder;
import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.opengl.EGLContext;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class OgreRenderer implements GLSurfaceView.Renderer
{
    private SurfaceHolder holder = null;
    private long nativeGvrContext = 0;
    private boolean surfaceCreated = false;
    private AssetManager assetMgr = null;
    
    public OgreRenderer(SurfaceHolder holder, long nativeGvrContext, AssetManager amgr) {
        Log.i("OgreRenderer", "constructor holder: " + holder);
        this.holder = holder;
        Log.i("OgreRenderer", "constructor surface: " + holder.getSurface());
        this.nativeGvrContext = nativeGvrContext;
        this.assetMgr = amgr;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i("OgreRenderer", "onSurfaceCreated");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i("OgreRenderer", "onSurfaceChanged " + width + "x" + height );
        if (!surfaceCreated) {
            Log.i("OgreRenderer", "onSurfaceChanged holder: " + holder);
            Log.i("OgreRenderer", "onSurfaceChanged surface: " + holder.getSurface());
            Log.i("OgreRenderer", "onSurfaceChanged this.holder surface: " + this.holder.getSurface());
            //AssetManager assetMgr = getResources().getAssets();
	    Native.InitOgre(holder.getSurface(), nativeGvrContext, assetMgr);

            surfaceCreated = true;
            return;
        }
        Log.e("OgreClient", "Surface change not supported");
        //throw new RuntimeException("Surface change not supported");
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Log.i("OgreRenderer", "onDrawFrame");
	// int[] value = new int[1];
	// EGLDisplay dpy = EGL14.eglGetCurrentDisplay();
	// EGLContext ctx = EGL14.eglGetCurrentContext();
	// EGL14.eglQueryContext(dpy, ctx, EGL14.EGL_CONTEXT_CLIENT_TYPE, value, 0);
	// Log.i("OgreRenderer", String.format("EGL client type: %d", value[0]));
	// EGL14.eglQueryContext(dpy, ctx, EGL14.EGL_CONTEXT_CLIENT_VERSION, value, 0);
	// Log.i("OgreRenderer", String.format("EGL client version: %d", value[0]));
        Native.Render();
    }
}
