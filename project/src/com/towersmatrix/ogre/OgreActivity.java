package com.towersmatrix.ogre;

import android.app.Activity;
import android.os.Bundle;

import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.View;
import android.view.WindowManager;

import com.google.vr.ndk.base.AndroidCompat;
import com.google.vr.ndk.base.GvrLayout;
import com.google.vr.ndk.base.GvrUiLayout;


public class OgreActivity extends Activity {
    static {
        System.loadLibrary("ogreclient");
    }
    
    private GvrLayout gvrLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        // Ensure fullscreen immersion.
        setImmersiveSticky();
        getWindow()
            .getDecorView()
            .setOnSystemUiVisibilityChangeListener
	    ( new View.OnSystemUiVisibilityChangeListener() {
		    @Override
		    public void onSystemUiVisibilityChange(int visibility) {
			if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
			    setImmersiveSticky();
			}
		    }
		});

        // Initialize GvrLayout
        gvrLayout = new GvrLayout(this);

        // Add the GLSurfaceView to the GvrLayout.
        GLSurfaceView glSurfaceView = new GLSurfaceView(this);
        glSurfaceView.setEGLContextClientVersion(2);
        glSurfaceView.setEGLConfigChooser(8, 8, 8, 0, 16, 8);
        glSurfaceView.setPreserveEGLContextOnPause(true);
        glSurfaceView.setRenderer(new OgreRenderer(glSurfaceView.getHolder(),
                                                   gvrLayout.getGvrApi().getNativeGvrContext(),
                                                   getResources().getAssets()));
        gvrLayout.setPresentationView(glSurfaceView);

        // Add the GvrLayout to the View hierarchy.
	setContentView(gvrLayout);
	//setContentView(glSurfaceView);
        // Enable scan line racing.
        // if (gvrLayout.setAsyncReprojectionEnabled(true)) {
        //     // Scanline racing decouples the app framerate from the display framerate,
        //     // allowing immersive interaction even at the throttled clockrates set by
        //     // sustained performance mode.
        //     AndroidCompat.setSustainedPerformanceMode(this, true);
        // }

        // Enable VR Mode.
        AndroidCompat.setVrModeEnabled(this, true);
        Log.i("OgreClient", "glSurfaceView dimensions "
              + glSurfaceView.getWidth()
              + "x"
              + glSurfaceView.getHeight());

        // Prevent screen from dimming/locking.
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.i("OgreClient", "onPause");
        gvrLayout.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i("OgreClient", "onResume");
        gvrLayout.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Destruction order is important; shutting down the GvrLayout will detach
        // the GLSurfaceView and stop the GL thread, allowing safe shutdown of
        // native resources from the UI thread.

        gvrLayout.shutdown();
        Log.i("OgreClient", "onDestroy");
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            setImmersiveSticky();
        }
    }

    private void setImmersiveSticky() {
        getWindow()
            .getDecorView()
            .setSystemUiVisibility(
                                   View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                   | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                   | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                   | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                   | View.SYSTEM_UI_FLAG_FULLSCREEN
                                   | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch (keyCode) {
        case KeyEvent.KEYCODE_A:
            Native.HandleKeyDown(1);
            return true;
        case KeyEvent.KEYCODE_D:
            Native.HandleKeyDown(2);
            return true;
        case KeyEvent.KEYCODE_W:
            Native.HandleKeyDown(3);
            return true;
        case KeyEvent.KEYCODE_S:
            Native.HandleKeyDown(4);
            return true;
        default:
            return super.onKeyDown(keyCode, event);
        }
    }

        @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        switch (keyCode) {
        case KeyEvent.KEYCODE_A:
            Native.HandleKeyUp(1);
            return true;
        case KeyEvent.KEYCODE_D:
            Native.HandleKeyUp(2);
            return true;
        case KeyEvent.KEYCODE_W:
            Native.HandleKeyUp(3);
            return true;
        case KeyEvent.KEYCODE_S:
            Native.HandleKeyUp(4);
            return true;
        default:
            return super.onKeyUp(keyCode, event);
        }
    }
}
