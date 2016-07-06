#include <jni.h>
#include "ColonyCalculator.h"
#include "RollingBall.h"
#include<android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "libnav", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "libnav", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "libnav", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "libnav", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "libnav", __VA_ARGS__)

using namespace std;
using namespace cv;


extern "C" {

JNIEXPORT jint JNICALL Java_com_example_chk_countinglogix_MainActivity_FindColony
        (JNIEnv *, jobject, jlong, jlong, jlong, jint, jboolean, jint);

JNIEXPORT void JNICALL Java_com_example_chk_countinglogix_MainActivity_MakeBinaryImage(JNIEnv *, jobject, jlong, jint);

JNIEXPORT void JNICALL Java_com_example_chk_countinglogix_MainActivity_RollingBall(JNIEnv *, jobject, jlong, jint);

JNIEXPORT void JNICALL Java_com_example_chk_countinglogix_MainActivity_AddCoordinates(JNIEnv *, jobject, jlong addrOrigin,  jint realX, jint realY);

JNIEXPORT jint JNICALL Java_com_example_chk_countinglogix_MainActivity_DeleteCoordinates(JNIEnv *, jobject, jlong addrOrigin,  jint realX, jint realY);

JNIEXPORT jint JNICALL Java_com_example_chk_countinglogix_MainActivity_FindColony(JNIEnv* env, jobject,
jlong addrRgb, jlong addrGray, jlong addrDish, jint addThreshold, jboolean isReverse, jint radius)
{
    int count = ColonyCalculator::CalculateColony(addrRgb, addrGray, addrDish, addThreshold, isReverse, radius);
    return  count;
}

JNIEXPORT void JNICALL Java_com_example_chk_countinglogix_MainActivity_AddCoordinates(JNIEnv *, jobject, jlong addrOrigin,  jint realX, jint realY){
    ColonyCalculator::AddCoordinates(addrOrigin, realX, realY);
}

JNIEXPORT jint JNICALL Java_com_example_chk_countinglogix_MainActivity_DeleteCoordinates(JNIEnv *, jobject, jlong addrOrigin,  jint realX, jint realY){
    return ColonyCalculator::DeleteCoordinates(addrOrigin, realX, realY);
}

JNIEXPORT void JNICALL Java_com_example_chk_countinglogix_MainActivity_RollingBall(JNIEnv *, jobject, jlong addrGray,  jint radius){
    Mat& mat_gray  = *(Mat*)addrGray;
    IplImage* _gray = new IplImage(mat_gray);

    RollingBall::subtractBackground( _gray, _gray, radius);
}


JNIEXPORT void JNICALL Java_com_example_chk_countinglogix_MainActivity_MakeBinaryImage(JNIEnv *, jobject, jlong addrGray,  jint addThreshold){
    ColonyCalculator::MakeBinaryImage(addrGray, addThreshold);
}

}


