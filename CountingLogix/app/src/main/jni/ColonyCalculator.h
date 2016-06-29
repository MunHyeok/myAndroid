//
// Created by CHK on 2016-04-05.
//

#ifndef ANDROID_COLONYCALCULATOR_H
#define ANDROID_COLONYCALCULATOR_H
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include "ColonyTool.h"
#include "ColonyCell.h"
#include "cimUtils.h"
#include "Blob.h"
#include<android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "libnav", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "libnav", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "libnav", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "libnav", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "libnav", __VA_ARGS__)

using namespace std;
using namespace cv;

#define	MAX_NUM_CLUSTER	2048
#define DIST(x,y)	((x)>(y)) ? ((x)-(y)) : ((y)-(x))
static	float	_mean[MAX_NUM_CLUSTER];
static	int		_cnt[MAX_NUM_CLUSTER];

float* cimKMeans1D(float *p, int len, int k)
{
	if (p == NULL || len <= k) return NULL;

	float	posi[MAX_NUM_CLUSTER];
	bool	bChanged = true;
	float	mind, dst, d, mean;
	int		i, n, mink;

	// 최초 cluster 별 평균값 초기화
	for (i = 0; i<k; i++)
		_mean[i] = p[i];


	while (bChanged) {
		for (i = 0; i<k; i++) {
			_cnt[i] = 0;
			posi[i] = 0.0f;
		}

		for (n = 0; n<len; n++) {
			d = p[n];
			mink = 0;
			mind = DIST(d, _mean[mink]);

			// 가장 가까운 평균을 찾음
			for (i = 1; i<k; i++) {
				dst = DIST(d, _mean[i]);
				if (mind > dst) {
					mind = dst;
					mink = i;
				}
			}

			// 새로운 평균 계산을 위해 해당 위치의 값 및 count 추가
			posi[mink] += d;
			_cnt[mink]++;
		}

		// 평균값 변경 : 변경이 있으면 ==> 다시 반복
		bChanged = false;
		for (i = 0; i<k; i++) {
			mean = posi[i] / _cnt[i];
			if (_mean[i] > mean + 0.000001f || _mean[i] < mean - 0.000001f) {
				bChanged = true;
				_mean[i] = mean;
			}
		}
	}
	return _mean;
}
float cimKMeansMin(float *p, int len, int k)
{
	float	*mean = cimKMeans1D(p, len, k);

	if (mean != NULL) {
		int min = 0;
		for (int i = 1; i < k; i++){
			if (mean[min] > mean[i]) min = i;
		}

		return mean[min];

	}
	else return 0.0f;
}

class ColonyCalculator{
private:
    static CColonyCell* m_cell;
    static int thres;
    static int m_nCells;
    static void setImage(IplImage* _gray, IplImage* _bin, IplImage* _dst, IplImage* _dish, int addThreshold)
    {
        if(addThreshold== 0)
        {
            thres = cvRound(cvThreshold( _bin, _bin, 0, 255, CV_THRESH_OTSU));
        }
        else
        {
            thres += addThreshold;
            cvThreshold( _bin, _bin, thres, 255, CV_THRESH_BINARY);
        }

        cvMorphologyEx( _bin, _bin, 0, 0, CV_MOP_OPEN, 3 );


        cvMin(_bin, _dish, _bin);

        CBlobAreas* blobAreas = CBlob::FindBlobAreas(_bin);
        double data = (double)cimKMeansMin(blobAreas->data, blobAreas->length, 3);
        int radius = (int)sqrt(data / (3.141592));

        cvCreateSmart (  cvSize(_gray->width, _gray->height), 32, 1, &_dst );
        cvDistTransform	 ( _bin, _dst);
        cvNormalize		 ( _dst, _dst, 0, 1., cv::NORM_MINMAX);

        cvSmooth(_dst, _dst, CV_GAUSSIAN, radius * 2 - 1);
        cvLocalMax32f	 ( _dst, _gray,(int)radius/4, 0.1f);
        //cvTrimLocalMax32f( _dst, _gray, 11 );

    }
    static int RegisterCells( IplImage *max)
    {
        m_nCells = 0;
        m_cell = new CColonyCell[10000];
        for( int y=0 ; y<cvHeight(max) ; y++ ) {
            unsigned char *c = cvGetPixelPtr(max, 0, y );
            for( int x=0 ; x<cvWidth(max) ; x++ ) {
                if( c[x] == 255 )
                    m_cell[m_nCells++].Set( x, y, 7.f );
                else if ( c[x] > 10 )
                    m_cell[m_nCells++].Set( x, y, 3.f, false );
            }
        }
        return m_nCells;
    }

    static int AddCell(int realX, int realY){
        m_cell[m_nCells++].Set(realX, realY, 7.f);
        return m_nCells;
    }
public:

    static int CalculateColony(jlong addrRgb, jlong addrGray, jlong addrDish, jint addThreshold)
    {
        Mat& mat_colony  = *(Mat*)addrRgb;
        Mat& mat_gray  = *(Mat*)addrGray;
        Mat& mat_dish  = *(Mat*)addrDish;
        Mat mat_bin = mat_gray.clone();
        Mat mat_lmax = mat_gray.clone();

        IplImage *_dst = NULL;
        IplImage *_colony = new IplImage(mat_colony);
        IplImage *_gray = new IplImage(mat_gray);
        IplImage *_bin = new IplImage(mat_bin);
        IplImage *_dish = new IplImage(mat_dish);

        setImage(_gray, _bin, _dst, _dish, addThreshold);

        int nCells = RegisterCells( _gray );

        LOGI("nCells : %d", nCells);
        for(int i = 0; i < nCells; i++)
        {
            m_cell[i].Draw( _colony );
        }

        cvReleaseImage(&_dst);
        return nCells;
    }

    static void AddCoordinates(jlong addrRgb, jint realX, jint realY){
        Mat& mat_colony  = *(Mat*)addrRgb;
        IplImage *_colony = new IplImage(mat_colony);
        int nCells = AddCell(realX, realY);
        for(int i = 0; i < nCells; i++)
        {
            m_cell[i].Draw( _colony );
        }
    }
    static void MakeBinaryImage(jlong addrGray, jint addThreshold){
        Mat& mat_gray  = *(Mat*)addrGray;
        IplImage *_gray = new IplImage(mat_gray);
        if(addThreshold== 0)
        {
            thres = cvRound(cvThreshold( _gray, _gray, 50, 255, CV_THRESH_OTSU));
        }
        else
        {
            thres += addThreshold;
            cvThreshold( _gray, _gray, thres, 255, CV_THRESH_BINARY);
        }
    }

};


int ColonyCalculator::thres = 0;
CColonyCell* ColonyCalculator::m_cell =  new CColonyCell[10000];
int ColonyCalculator::m_nCells = 0;
#endif //ANDROID_COLONYCALCULATOR_H
