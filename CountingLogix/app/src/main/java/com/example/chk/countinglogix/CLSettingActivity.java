package com.example.chk.countinglogix;

import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.Toast;

/**
 * Created by CHK on 2016-05-01.
 */
public class CLSettingActivity extends CLActivity{
    RadioButton rb_circle;
    RadioButton rb_rectangle;
    EditText txt_thres;
    EditText txt_radius;
    Button btn_ok;
    Button btn_cancel;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        initVariables();
        setData();
        setEvents();
    }

    private void initVariables() {
        btn_ok = (Button)findViewById(R.id.btn_ok);
        btn_cancel = (Button)findViewById(R.id.btn_cancel);
        rb_circle = (RadioButton)findViewById(R.id.rb_circle);
        rb_rectangle = (RadioButton)findViewById(R.id.rb_rectangle);
        txt_thres = (EditText)findViewById(R.id.txt_thres);
        txt_radius = (EditText)findViewById(R.id.txt_radius);
    }
    private void setData() {
        txt_thres.setText(Integer.toString(GlobalInfo.thresChange));
        txt_radius.setText(Integer.toString(GlobalInfo.radius));
        if(GlobalInfo.isCircle)
            rb_circle.setChecked(true);
        else
            rb_rectangle.setChecked(true);
    }
    private void setEvents(){
        btn_cancel.setOnClickListener(finishClickListener);
        btn_ok.setOnClickListener(finishClickListener);
    }

    View.OnClickListener finishClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId())
            {
                case R.id.btn_ok :
                {

                    if(isValidText()){
                        if(rb_circle.isChecked())
                            GlobalInfo.isCircle = true;
                        else if(rb_rectangle.isChecked())
                            GlobalInfo.isCircle = false;

                        GlobalInfo.radius = Integer.parseInt(txt_radius.getText().toString());
                        GlobalInfo.thresChange = Integer.parseInt(txt_thres.getText().toString());
                        finish();
                    }

                    break;
                }
                case R.id.btn_cancel:
                {
                    finish();
                    break;
                }
            }
        }

        boolean isValidText(){
            if(Integer.parseInt(txt_radius.getText().toString()) <= 0){
                Toast toast = Toast.makeText(getApplicationContext(),
                        "음수는 적용될 수 없습니다.", Toast.LENGTH_LONG);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
                return false;
            }


            if(!txt_thres.getText().toString().equals("") && !txt_radius.getText().toString().equals(""))
                return true;
            else
                return false;
        }
    };


}
