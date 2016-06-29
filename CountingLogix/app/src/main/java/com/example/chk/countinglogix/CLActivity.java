package com.example.chk.countinglogix;

import android.app.Activity;
import android.os.Bundle;
import android.util.DisplayMetrics;

/**
 * Created by CHK on 2016-05-19.
 */
public class CLActivity extends Activity {
    protected void getDeviceInches(){
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        float yInches= metrics.heightPixels/metrics.ydpi;
        float xInches= metrics.widthPixels/metrics.xdpi;
        GlobalInfo.diagonalInches = Math.sqrt(xInches*xInches + yInches*yInches);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getDeviceInches();
        if (GlobalInfo.diagonalInches>=6.5){
            setTheme(android.R.style.Theme_Holo_Light);

        }else{
            setTheme(android.R.style.Theme_NoTitleBar_Fullscreen);
        }
        super.onCreate(savedInstanceState);
    }

    public int getStatusBarHeight() {
        int result = 0;
        int resourceId = getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = getResources().getDimensionPixelSize(resourceId);
        }
        return result;
    }
}
