package com.example.chk.countinglogix;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by CHK on 2016-05-01.
 */
public class CLCameraActivity extends CLActivity implements SensorEventListener {

    static final String	RESULT_SAVE_FILENAME 		= "RESULT_SAVE_FILENAME";
    static final int 	RESULT_ERROR_CAMERA_DIED	= 9100;
    static final int 	RESULT_ERROR_UNKNOWN		= 9999;
    private Camera _camera = null;
    private byte[] 	FrameData 		= null;
    private boolean _bUsingFocus 	= false;
    private boolean _isFocused 		= false;
    private boolean	_isImageSaving 	= false; // 이미지 저장중 여부
    private boolean _isTakePicture 	= false;

    private SurfaceView svPreview = null;
    private SurfaceHolder _shPreviewHolder = null;
    ProgressDialog _progressDialog = null;
    private SensorManager _sensorManager = null;
    private Sensor _OriSensor, _AccelerSensor, _MagneticSensor;
    private boolean 	bProcessing	= false;
    Handler mHandler = new Handler(Looper.getMainLooper());

    @SuppressWarnings("deprecation")
    @SuppressLint({ "NewApi", "InlinedApi" })
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);


        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        _camera = Camera.open();
        svPreview = (SurfaceView)findViewById(R.id.svPreviewCamera);
        _shPreviewHolder = svPreview.getHolder();
        _shPreviewHolder.addCallback(surfaceCallback);
        _shPreviewHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        Button btn_capture = (Button)findViewById(R.id.btn_capture);
        btn_capture.setOnLongClickListener(onLongClickListener_AutoFocus);
        btn_capture.setOnClickListener(_onClickListener);



    }


    private View.OnLongClickListener onLongClickListener_AutoFocus = new View.OnLongClickListener() {

        @Override
        public boolean onLongClick(View v) {
            if(_camera != null && isAutoFocusingCamera(_camera)){
                _bUsingFocus = true;
                _isFocused = false;
                try{
                    _camera.autoFocus(autoFocusCallback);
                }
                catch(Exception e){
                    System.out.println(e.toString());
                }
            }
            return false;
        }
    };

    private View.OnClickListener _onClickListener = new View.OnClickListener() {
        @SuppressLint("ShowToast")
        @Override
        public void onClick(View v) {
            bProcessing = true;
            switch(v.getId()){
                case R.id.btn_capture:{
                    // 이미지가 저장중이 아니고, 포커스를 사용안할 경우나, 포커스를 사용할때 포커스 완료됐을 때 촬영s
                    if( (_isImageSaving == false && _isFocused) || (_isImageSaving == false && !_bUsingFocus)){
                        _camera.takePicture(shutterCallback, photoCallback_RAW, photoCallback_JPG);

                    }
                    else _isTakePicture = true; // 오토포커싱 콜백 함수에서 사진 촬영
                }
                break;
            }
        }
    };



    Camera.ShutterCallback shutterCallback = new Camera.ShutterCallback(){
        @Override
        public void onShutter() {}
    };

    private Camera.AutoFocusCallback autoFocusCallback = new Camera.AutoFocusCallback() {
        @Override
        public void onAutoFocus(boolean bSuccess, Camera camera) {
            if(_isImageSaving == false && _isTakePicture){
                _camera.takePicture(shutterCallback, photoCallback_RAW, photoCallback_JPG);
                _isTakePicture = false;
            }
            _isFocused = true;
            _bUsingFocus = false;
            String msg = String.format("AutoFocus : %s", Boolean.toString(bSuccess));
            System.out.println(msg);
        }

    };

    public void showProgress(){		 	// 프로그래스 다이얼로그를 보인다.
        if(_progressDialog == null){
            _progressDialog = new ProgressDialog(this);
            _progressDialog.setMessage("이미지를 저장하고 있습니다.");
            _progressDialog.setCancelable(false);
        }
        _progressDialog.show();
    }
    public void hideProgress(){			// 프로그래스 다이얼로그를 숨긴다.
        if(_progressDialog != null){
            _progressDialog.dismiss();
            _progressDialog = null;
        }
    }

    class SavePhotoTask extends AsyncTask<byte[], String, String> {
        @Override
        protected void onPreExecute() {
            // 시작
            super.onPreExecute();
            showProgress();
        }
        @Override
        protected void onPostExecute(String result) {
            hideProgress();
            // 종료
            super.onPostExecute(result);
            _camera.startPreview();
        }


        @Override
        protected String doInBackground(byte[]... jpeg) {

            _isImageSaving = true;

            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inJustDecodeBounds = true;

            GlobalInfo.CLCameraBitmap = BitmapFactory.decodeByteArray(jpeg[0], 0, jpeg[0].length);
            String st = Environment.getExternalStorageDirectory() + "/DCIM/COLONY";

            File photo = new File(st);

            if(!photo.isDirectory()) {
                photo.mkdirs();
            }
            st = photo.getPath();
            FileOutputStream stream = null;
            try {
                stream = new FileOutputStream(st + "/test.jpg");
                if( stream != null) {
                    GlobalInfo.CLCameraBitmap.compress(Bitmap.CompressFormat.JPEG, 100, stream);
                    stream.close();
                    Intent intent = new Intent(CLCameraActivity.this, MainActivity.class);
                    intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                    startActivity(intent);

                }
            }
            catch (FileNotFoundException e) { 		// TODO Auto-generated catch block
                e.printStackTrace();
            }
            catch (IOException e) {					// TODO Auto-generated catch block
                e.printStackTrace();
            }
            Intent i = new Intent();
            i.putExtra(RESULT_SAVE_FILENAME, st);
            setResult(RESULT_OK, i);

            _isImageSaving = false;

            return(null);
        }
    }

    Camera.PictureCallback photoCallback_JPG =new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            (new SavePhotoTask()).execute(new byte[][]{data});
        }
    };

    Camera.PictureCallback photoCallback_RAW = new Camera.PictureCallback(){
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
        }
    };

    private boolean isAutoFocusingCamera(Camera camera)
    {
        // 카메라 파라미터 불러오기
        Camera.Parameters parameters = camera.getParameters();

        // 카메라 파라미터 설정
        List<String> focusModes = parameters.getSupportedFocusModes(); // 자동 초점
        if (focusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO))
            return true;
        return false;
    }

    SurfaceHolder.Callback surfaceCallback=new SurfaceHolder.Callback()
    {
        /**
         * 카메라 뷰어 생성
         */
        @SuppressWarnings("deprecation")
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            try {
                // 화면 뷰 설정

                _camera.setPreviewDisplay(_shPreviewHolder);
                _camera.setPreviewCallback(_previewCallback);
            }
            catch(Exception e) {
                setResult(RESULT_ERROR_UNKNOWN);
                finish();
            }
        }


        @SuppressLint("NewApi")
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
        {
            // 카메라 파라미터 불러오기
            Camera.Parameters parameters = _camera.getParameters();

            List<Camera.Size> arSize = parameters.getSupportedPreviewSizes();
            if(arSize==null){
                parameters.setPreviewSize(width, height);
                String msg;
                msg= "width:"+width+"height:"+height;
                Log.i("tag", msg);
            }

            else {
                int diff = 10000;
                Camera.Size opti = null;
                for(Camera.Size s : arSize){
                    if(Math.abs(s.height - height) < diff) {
                        diff = Math.abs(s.height - height);
                        opti = s;
                    }
                }
                parameters.setPreviewSize(opti.width, opti.height);
            }

            _camera.setParameters(parameters);
            _camera.setErrorCallback(errorCallback);
            // 에러 콜백 함수 설정
            _camera.startPreview();
            // 프리뷰 시작
        }

        /**
         * 뷰 종료시
         */
        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            Log.i(getApplicationContext().getResources().getString(R.string.app_name), "surfaceDestroyed");
        }
    };

    Camera.ErrorCallback errorCallback = new Camera.ErrorCallback() {

        @Override
        public void onError(int errorCode, Camera camera) {
            if(errorCode == Camera.CAMERA_ERROR_UNKNOWN)
            {
                System.out.println("알수 없는 에러 발생!!!");
                setResult(RESULT_ERROR_UNKNOWN);
                finish();
            }
            else if(errorCode == Camera.CAMERA_ERROR_SERVER_DIED)
            {
                System.out.println("카메라가 종료됨!!!");
                setResult(RESULT_ERROR_CAMERA_DIED);
                finish();
            }
        }
    };

    Camera.PreviewCallback _previewCallback = new Camera.PreviewCallback() {

        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            // TODO Auto-generated method stub
            //We only accept the NV21(YUV420) format.
            if ( bProcessing )
            {
                FrameData = data;
//                mHandler.post(DoImageProcessing);
            }
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(this._camera != null) {
            this._camera.stopPreview();
            new Timer().schedule(new TimerTask() {
                @Override
                public void run() {
                    _camera.release();
                }
            }, 200);

        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {

    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}
