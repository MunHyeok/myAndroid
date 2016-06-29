package com.example.chk.countinglogix;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.provider.MediaStore;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import java.io.FileNotFoundException;
import java.io.IOException;

import uk.co.senab.photoview.PhotoViewAttacher;

public class MainActivity extends CLActivity
{
    public PhotoViewAttacher mAttacher;
    private ImageView iv_colony;
    private TextView tv_count;
    private TextView tv_thres;
    private boolean rangeBtnRunning = false;
    private boolean isBinary = false;
    private boolean addBtnRunning = false;

    private  boolean imageExist = false;
    private int size;
    private int sum_thres_colony;
    private int sum_thres_bin;
    private Intent intent;
    private Bitmap image_bitmap;
    private AlertDialog.Builder alert;
    private double realX = 0;
    private double realY = 0;
    final int REQ_CODE_SELECT_IMAGE=100;

private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
    @Override
    public void onManagerConnected(int status) {
        switch (status) {
            case LoaderCallbackInterface.SUCCESS:
            {
                System.loadLibrary("CountingLogix");
            } break;
            default:
            {
                super.onManagerConnected(status);
            } break;
        }
    }
};


    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {


        super.onCreate(savedInstanceState);
        //getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);
        InitVariables(); // 변수 초기화

        Button btn_menu = (Button)findViewById(R.id.btn_menu);
        if (GlobalInfo.diagonalInches>=6.5){
            btn_menu.setVisibility(View.GONE);

        }else{

            btn_menu.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    MainActivity.this.openOptionsMenu();
                }
            });

        }


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.mymenu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle item selection
        switch (item.getItemId()) {
            case R.id.item_reverse:
                if (imageExist) {
                    Bitmap bmp_colony = image_bitmap.copy(image_bitmap.getConfig(), true);
                    image_bitmap = createInvertedBitmap(bmp_colony);
                    iv_colony.setImageBitmap(image_bitmap);
                }
                else{
                    noPictrueToast();
                }
                return true;
            case R.id.item_binary:
                if (imageExist && !rangeBtnRunning && !addBtnRunning) {
                    initThreshold();
                    makeBinary(0);
                    isBinary = true;
                }
                else{
                    invalidClickToast();
                }
                return true;
            case R.id.item_camera:
                if(!rangeBtnRunning && !addBtnRunning){
                    Intent intentCamera = new Intent(MainActivity.this, CLCameraActivity.class);
                    intentCamera.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                    startActivity(intentCamera);
                }
                else{
                    invalidClickToast();
                }
                return true;
            case R.id.item_album:
                if (!rangeBtnRunning && !addBtnRunning) {
                    intent = new Intent(Intent.ACTION_PICK);
                    intent.setType(android.provider.MediaStore.Images.Media.CONTENT_TYPE);
                    intent.setData(android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                    startActivityForResult(intent, REQ_CODE_SELECT_IMAGE);
                } else {
                    invalidClickToast();
                }
                return true;
            case R.id.item_setting:
                if(!rangeBtnRunning && !addBtnRunning){
                    Intent intentSetting = new Intent(MainActivity.this, CLSettingActivity.class);
                    startActivity(intentSetting);
                }
                else{
                    invalidClickToast();

                }
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void makeBinary(int thres) {
        Bitmap bmp_colony = image_bitmap.copy(image_bitmap.getConfig(), true);
        Mat mat_gray = new Mat();
        Utils.bitmapToMat(bmp_colony, mat_gray);
        Imgproc.cvtColor(mat_gray, mat_gray, Imgproc.COLOR_BGR2GRAY);
        MakeBinaryImage(mat_gray.getNativeObjAddr(), thres);
        Utils.matToBitmap(mat_gray, bmp_colony);
        iv_colony.setImageBitmap(bmp_colony);
        sum_thres_bin += thres;
        String str_thres = String.valueOf(sum_thres_bin);
        tv_thres.setText("Threshold = " + str_thres);

    }
    private Bitmap createInvertedBitmap(Bitmap src) {
        ColorMatrix colorMatrix_Inverted = new ColorMatrix(new float[] {
                        -1,  0,  0,  0, 255,
                        0, -1,  0,  0, 255,
                        0,  0, -1,  0, 255,
                        0,  0,  0,  1,   0});

        ColorFilter ColorFilter_Sepia = new ColorMatrixColorFilter(
                colorMatrix_Inverted);

        Bitmap bitmap = Bitmap.createBitmap(src.getWidth(), src.getHeight(),
                Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);

        Paint paint = new Paint();

        paint.setColorFilter(ColorFilter_Sepia);
        canvas.drawBitmap(src, 0, 0, paint);

        return bitmap;
    }
    private void InitVariables() {
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        size = 300;
        sum_thres_colony = 0;
        sum_thres_bin = 0;
        Handler mHandler = new Handler();
        mHandler.postDelayed(mMyTask, 200); //비동기 처리를 위한 임시방편

        iv_colony = (ImageView) findViewById(R.id.iv_colony);
        tv_count = (TextView)findViewById(R.id.tv_count);
        tv_thres = (TextView)findViewById(R.id.tv_thres);


        tv_thres.setText(String.format("Threshold = " + sum_thres_colony));
//
//        //경고메세지 세팅
//        alert = new AlertDialog.Builder(MainActivity.this);
//        alert.setPositiveButton("확인", new DialogInterface.OnClickListener() {
//            @Override
//            public void onClick(DialogInterface dialog, int which) {
//                dialog.dismiss();     //닫기
//            }
//        });
//        alert.setMessage("먼저 사진을 찍거나, 앨범에서 사진을 선택해 주세요.");

       mAttacher = new PhotoViewAttacher(iv_colony);
    }

    private void initThreshold(){
        sum_thres_colony = 0;
        sum_thres_bin = 0;
    }
    ImageView.OnTouchListener touch = new ImageView.OnTouchListener(){

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch(event.getAction()){
                case MotionEvent.ACTION_DOWN:
                {
                    DrawWhenTouched(event);
                    break;
                }
            }
            return false;
        }
    };

    ImageView.OnTouchListener touchedCoordinate = new ImageView.OnTouchListener(){

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch(event.getAction()){
                case MotionEvent.ACTION_DOWN:
                {
                    AddTouchedCoordinate(event);
                    break;
                }
            }
            return false;
        }
    };
    private Runnable mMyTask = new MyRunnable();

    private void DrawWhenTouched(MotionEvent event){
        int[] viewCoords = new int[2];
        iv_colony.getLocationOnScreen(viewCoords);
        realX = (int) event.getX();
        realY = (int) event.getY();

        if(GlobalInfo.isCircle)
            drawCircle();
        else
            drawRect();
    }

    private void AddTouchedCoordinate(MotionEvent event){
        int[] viewCoords = new int[2];
        iv_colony.getLocationOnScreen(viewCoords);
        realX = (int) event.getX();
        realY = (int) event.getY();

        Mat mat_colony = new Mat();
        Utils.bitmapToMat(image_bitmap, mat_colony);
        AddCoordinates(mat_colony.getNativeObjAddr(), (int)realX, (int)realY);
        Utils.matToBitmap(mat_colony, image_bitmap);
        iv_colony.setImageBitmap(image_bitmap);
    }


    private void drawCircle() {
        Bitmap bmp_colony = image_bitmap.copy(image_bitmap.getConfig(), true);
        Mat mat_colony = new Mat();
        Utils.bitmapToMat(bmp_colony, mat_colony);

        makeCircle(mat_colony, 3);
        Utils.matToBitmap(mat_colony, bmp_colony);
        iv_colony.setImageBitmap(bmp_colony);
    }

    private void drawRect() {
        Bitmap bmp_colony = image_bitmap.copy(image_bitmap.getConfig(), true);
        Mat mat_colony = new Mat();
        Utils.bitmapToMat(bmp_colony, mat_colony);
        makeRect(mat_colony, 5);
        Utils.matToBitmap(mat_colony, bmp_colony);
        iv_colony.setImageBitmap(bmp_colony);
    }

    private void FindColony(int addThreshold) {

        Bitmap bmp_colony = image_bitmap.copy(image_bitmap.getConfig(), true);
        Mat mat_colony = new Mat();
        Mat mat_gray = new Mat();
        Mat mat_dish = new Mat();

        Setting(mat_colony, mat_gray, mat_dish, bmp_colony);
        if(GlobalInfo.isCircle)
            makeCircle(mat_dish, -5);
        else
            makeRect(mat_dish, -5);


        int count = FindColony(mat_colony.getNativeObjAddr(), mat_gray.getNativeObjAddr(), mat_dish.getNativeObjAddr(), addThreshold);

        image_bitmap = bmp_colony;
        DrawView(mat_colony, count);
    }

    private void Setting(Mat mat_colony, Mat mat_gray, Mat mat_dish, Bitmap bmp_colony){
        Utils.bitmapToMat(bmp_colony, mat_colony);
        Utils.bitmapToMat(bmp_colony, mat_gray);
        Utils.bitmapToMat(bmp_colony, mat_dish);
        Imgproc.cvtColor(mat_colony, mat_gray, Imgproc.COLOR_BGR2GRAY);
        Imgproc.cvtColor(mat_colony, mat_dish, Imgproc.COLOR_BGR2GRAY);

        mat_dish.setTo(new Scalar(0));
    }

    private void makeCircle(Mat mat_sample, int shift) {
        Imgproc.circle(mat_sample, new Point(realX, realY), size, new Scalar(255, 255, 255), shift);
    }

    private void makeRect(Mat mat_sample, int shift) {
        Imgproc.rectangle(mat_sample, new Point(realX - (size / 2), realY - (size / 2)), new Point(realX + (size / 2), realY + (size / 2)), new Scalar(255, 255, 255), shift);
    }

    private void DrawView(Mat mat_colony, int count) {
        Utils.matToBitmap(mat_colony, image_bitmap);
        iv_colony.setImageBitmap(image_bitmap);
        String str_colony = String.valueOf(count);
        tv_count.setText("Total Colony = " + str_colony);
    }

    private void noPictrueToast() {
        Toast toast = Toast.makeText(getApplicationContext(),
                "먼저 사진을 찍거나, 앨범에서 사진을 선택해 주세요.", Toast.LENGTH_LONG);
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }

    private void invalidAreaToast() {
        Toast toast = Toast.makeText(getApplicationContext(),
                "영역이 아닙니다.", Toast.LENGTH_LONG);
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }

    private void invalidClickToast() {
        Toast toast = Toast.makeText(getApplicationContext(),
                "실행중인 동작을 완료하고 설정을 변경해 주세요", Toast.LENGTH_LONG);
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }
    @Override
    public void onBackPressed() {
        super.onBackPressed();
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        if(GlobalInfo.CLCameraBitmap != null)
        {
            image_bitmap = GlobalInfo.CLCameraBitmap;
            SystemClock.sleep(100);
            resizeImage();
            SystemClock.sleep(100);
            iv_colony.setImageBitmap(image_bitmap);
            imageExist = true;
            isBinary = false;
        }
    }

    private void resizeImage(){

        int imageWidth = image_bitmap.getWidth();
        int imageHeight = image_bitmap.getHeight();
        int deviceWidth = Resources.getSystem().getDisplayMetrics().widthPixels;
        int deviceHeight;
        if (GlobalInfo.diagonalInches>=6.5){
            deviceHeight = Resources.getSystem().getDisplayMetrics().heightPixels - (getActionBar().getHeight() + getStatusBarHeight());

        }else{
            deviceHeight = Resources.getSystem().getDisplayMetrics().heightPixels;
        }


        if(imageWidth > deviceWidth || imageHeight > deviceHeight){

            if(imageHeight >= imageWidth){
                double resizeWidth = (double)imageWidth * (double)deviceHeight / (double)imageHeight;
                double resizeHeight = deviceHeight;
                image_bitmap = Bitmap.createScaledBitmap(image_bitmap, (int)resizeWidth, (int)resizeHeight, true);
            }
            else if(imageWidth > imageHeight){
                double resizeHeight = (double)imageHeight * (double)deviceWidth / (double)imageWidth;
                double resizeWidth = deviceWidth;
                image_bitmap = Bitmap.createScaledBitmap(image_bitmap, (int)resizeWidth, (int)resizeHeight, true);
            }
        }


    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == REQ_CODE_SELECT_IMAGE) {
            if (resultCode == Activity.RESULT_OK) {
                try {
                    image_bitmap = MediaStore.Images.Media.getBitmap(getContentResolver(), data.getData());
                    SystemClock.sleep(100);
                    resizeImage();
                    SystemClock.sleep(100);
                    iv_colony.setImageBitmap(image_bitmap);
                    imageExist = true;
                } catch (FileNotFoundException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public native int FindColony(long mat_origin, long mat_gray, long mat_dish, int addThreshold);
    public native void AddCoordinates(long mat_origin, int x, int y);
    public native void MakeBinaryImage(long mat_gray, int addThreshold);

    private class MyRunnable implements Runnable{


        @Override
        public void run() {

            final Button btn_range = (Button)findViewById(R.id.btn_range);
            btn_range.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(imageExist)//버튼이 사용가능한 상태면
                    {
                        if(addBtnRunning){
                            invalidClickToast();
                        }
                        else if(!rangeBtnRunning)//범위버튼이 작동하고 있는지 아닌지
                        {
                            mAttacher.cleanup();
                            iv_colony.setOnTouchListener(touch);
                            rangeBtnRunning = true;
                            isBinary = false;

                            btn_range.setText("완료");
                        }
                        else
                        {
                            iv_colony.setOnTouchListener(null);
                            mAttacher = new PhotoViewAttacher(iv_colony);
                            FindColony(0);
                            initThreshold();
                            rangeBtnRunning = false;
                            String str_thres = String.valueOf(sum_thres_colony);
                            tv_thres.setText("Threshold = " + str_thres);
                            btn_range.setText("범위지정");
                        }



                    }
                    else
                    {
                        noPictrueToast();
                    }
                }
            });

            final Button btn_add = (Button)findViewById(R.id.btn_add);
            btn_add.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(imageExist)//버튼이 사용가능한 상태면
                    {
                        if(rangeBtnRunning)//범위버튼이 작동하고 있는지 아닌지
                        {
                            invalidClickToast();
                        }
                        else if(btn_add.getText().equals("추가"))
                        {
                            mAttacher.cleanup();
                            iv_colony.setOnTouchListener(touchedCoordinate);
                            btn_add.setText("완료");
                            addBtnRunning = true;
                        }
                        else if(btn_add.getText().equals("완료"))
                        {
                            iv_colony.setOnTouchListener(null);
                            mAttacher = new PhotoViewAttacher(iv_colony);
                            btn_add.setText("추가");
                            addBtnRunning = false;
                        }
                    }
                    else
                    {
                        noPictrueToast();
                    }
                }
            });


            Button btn_plus = (Button)findViewById(R.id.btn_plus);
            btn_plus.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(imageExist)//버튼이 사용가능한 상태면
                    {
                        if(rangeBtnRunning)//범위버튼이 작동하고 있는지 아닌지
                        {
                            size += 30;
                            if(GlobalInfo.isCircle)
                                drawCircle();
                            else
                                drawRect();
                        }
                        else if(isBinary){
                            makeBinary(GlobalInfo.thresChange);
                        }
                        else
                        {
                            sum_thres_colony += GlobalInfo.thresChange;;
                            String str_thres = String.valueOf(sum_thres_colony);
                            tv_thres.setText("Threshold = " + str_thres);
                            FindColony(GlobalInfo.thresChange);
                        }
                    }
                    else
                    {
                        noPictrueToast();
                    }
                }
            });

            Button btn_minus = (Button)findViewById(R.id.btn_minus);
            btn_minus.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (imageExist)// 버튼이 사용가능한 상태면
                    {
                        if (rangeBtnRunning)//범위버튼이 작동하고 있는지 아닌지
                        {
                            if (size > 30) {
                                size -= 30;
                            }
                            if(GlobalInfo.isCircle)
                                drawCircle();
                            else
                                drawRect();
                        }
                        else if(isBinary){
                            makeBinary(-GlobalInfo.thresChange);

                        }
                        else{
                            sum_thres_colony += -GlobalInfo.thresChange;;
                            String str_thres = String.valueOf(sum_thres_colony);
                            tv_thres.setText("Threshold =" + str_thres);
                            FindColony(-GlobalInfo.thresChange);
                        }
                    }
                    else {
                        noPictrueToast();
                    }

                }
            });
        }
    }
}
