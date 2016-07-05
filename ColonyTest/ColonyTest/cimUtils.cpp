/*******************************************************************************
* Copyright (C) 2011 Young Kyu Choi,
*		CSE, Korea Univ. of Technology and Education
*		Tel: 019-224-8277
*		Email: ykchoi@kut.ac.kr
*
*  < OpenCV 함수의 편리한 활용을 위한 Utility > Implementation file
*    2011.02 :
*	- 영상내 글씨를 적기 위한 함수
*	-
*	-
*    2011.03.09 :
*	- floating point array를 처리하기 위한 함수들을 추가
*	-
*/
#include "cimUtils.h"


//=============================================================================
// Drawing functions 
//	- Rect
//	- Text
#define MaxNumFonts 8
static int	s_font[MaxNumFonts] = {
	CV_FONT_HERSHEY_SIMPLEX,
	CV_FONT_HERSHEY_PLAIN,
	CV_FONT_HERSHEY_DUPLEX,
	CV_FONT_HERSHEY_COMPLEX,
	CV_FONT_HERSHEY_TRIPLEX,
	CV_FONT_HERSHEY_COMPLEX_SMALL,
	CV_FONT_HERSHEY_SCRIPT_SIMPLEX,
	CV_FONT_HERSHEY_SCRIPT_COMPLEX
};

// draw text message
void cvDrawText(IplImage *src, const char *msg, CvPoint pt, int font, CvScalar color, float scale, int thick)
{
	if (src == NULL || msg == NULL) return;
	if (font<0 || font>MaxNumFonts) font = 0;

	CvFont fontface;
	cvInitFont(&fontface, s_font[font], scale, scale, 0, thick, CV_AA);
	cvPutText(src, msg, pt, &fontface, color);
}


float cimCalcAvgSmallDev(float *prj, int len, float avg, float th)
{
	if (prj == NULL || len<1) return -1.f;

	int	    count = 0;
	float   sum = 0.0f;
	float   diff;
	for (int i = 0; i<len; i++) {
		diff = (prj[i] > avg) ? (prj[i] - avg) : (avg - prj[i]);
		if (diff < th) {
			sum += prj[i];
			count++;
		}
	}
	return (count <= 0) ? -1.0f : sum / count;
}




void cimDistProjection(float *prj, int len, float *res, float pitch)
{
	if (prj == NULL || res == NULL) return;

	int iPitch = cvRound(pitch*1.5);
	for (int i = 0; i<iPitch; i++) {

		float	sum = 0.0f;
		int	count = 0;
		for (int j = i; j<len;) {
			sum += prj[j];
			count++;
			j = cvRound(i + count*pitch);
		}
		res[i] = 10 * sum / count;
	}
}

/*
// 함수 사용 주의: 잘못하면 프로그램이 종료될 수 있음
float cimFindLocalBMPosi( float *prj, int si, int ei )
{
float maxf	= cimFindLocalMaxPosi( prj, si, ei );
int	  maxi  = cvRound(maxf);
int	  win1	= cvRound(( ei - si ) * 0.03);
int	  win2	= cvRound(( ei - si ) * 0.1);
if( win1 < 5 ) win1 = 5;
if( win2 < 10) win2 = 10;

int	lmin, rmin;

for( lmin=maxi-1 ; lmin>si ; lmin-- ) {
if( prj[lmin] > prj[lmin+1] ) break;
}

for( rmin=maxi+1 ; rmin<ei ; rmin++ ) {
if( prj[rmin] > prj[rmin-1] ) break;
}

int	  lmax	= (int)cimFindLocalMaxPosi( prj, lmin-win2, lmin );
int	  rmax	= (int)cimFindLocalMaxPosi( prj, rmin,	 rmin+win2 );

int maxi2 = (prj[lmax] > prj[rmax] ) ? lmax : rmax;
if( prj[maxi2] > prj[maxi] * 0.6f )
return ((maxi+maxi2) * 0.5f);

// sub-pixel operation을 적용할 수 있음

return (float)maxi;
}
*/



/*
//===========================================================================
// static image handler
//
static IplImage *s_tmp = NULL;

// 영상처리와 관련된 모든 static 메모리들을 할당하고 해제하는 역할
class CImgStaticHandler {
public:
CImgStaticHandler () {
s_tmp = cvCreateImage( cvSize(1024, 1024), 8, 3 );
}
~CImgStaticHandler() {
cvReleaseImage( &s_tmp);
}
};

static CImgStaticHandler _theStaticHandler;

IplImage cvGetTmp( int w, int h, int x, int y )
{
return cvGetWindow( s_tmp, cvRect(x,y,w,h) );
}

*/

#include <time.h>
const char *cvGetCurrTime()
{
	static char str[400];
	time_t	curr;

	curr = time(NULL);		// 현재시각
	sprintf(str, "%ld", curr);
	return str;
}




bool cvCirclePPP(CvPoint2D32f& p1, CvPoint2D32f& p2, CvPoint2D32f& p3,
	CvPoint2D32f& center, float &radius)
{
	float bx = p1.x; float by = p1.y;
	float cx = p2.x; float cy = p2.y;
	float dx = p3.x; float dy = p3.y;
	float temp = cx*cx + cy*cy;
	float bc = (bx*bx + by*by - temp) / 2.0f;
	float cd = (temp - dx*dx - dy*dy) / 2.0f;
	float det = (bx - cx)*(cy - dy) - (cx - dx)*(by - cy);

	if (fabs(det) < 1.0e-6) {
		center.x = center.y = 1.0f;
		return false;
	}
	det = 1 / det;
	center.x = (bc*(cy - dy) - cd*(by - cy))*det;
	center.y = ((bx - cx)*cd - (cx - dx)*bc)*det;
	cx = center.x; cy = center.y;
	radius = sqrt((cx - bx)*(cx - bx) + (cy - by)*(cy - by));
	return true;
}

//======================================================================
//일단 무식하게 Local Maximum 을 찾는 함수
//		src는 입력 영상 (float 영상, 1channel)
//		dst는 결과 영상 (unsigned char, 1channel)
//		-linear 형태로 바꿀 필요 있음 (속도 향상)
//
extern bool cvTrimLocalMax32f(IplImage *src, IplImage *max, int dist);

bool cvLocalMax32f(IplImage *src, IplImage *dst, int dist, float th)
{
	if (!cvSameSize(src, dst)
		|| src->depth != 32 || dst->depth != 8
		|| src->nChannels != 1 || dst->nChannels != 1) return false;

	int w = cvWidth(src);
	int h = cvHeight(src);
	cvSetZero(dst);

	for (int y = dist; y<h - dist; y++) {

		float *p = cvGetFloat(src, 0, y);
		unsigned char *c = cvGetPixelPtr(dst, 0, y);

		for (int x = dist; x<w - dist; x++) {

			float& cen = p[x];
			if (cen < th) continue;
			bool isLocalMax = true;

			for (int i = y - dist; i <= y + dist; i++) {

				float *q = cvGetFloat(src, 0, i);

				for (int j = x - dist; j <= x + dist; j++) {

					if (q[j] > cen) {
						isLocalMax = false;
						i = y + dist;
						break;
					}
				}
			}

			// 하나의 local maximum point를 찾음
			if (isLocalMax) {
				//cen += (rand() % 100 * 0.000001f);
				c[x] = 255;
			}
		}
	}
	return true;
}

//======================================================================
//일단 무식하게 Local Maximum 을 찾는 함수
//		src는 입력 영상 (float 영상, 1channel)
//		dst는 결과 영상 (unsigned char, 1channel)
//		-linear 형태로 바꿀 필요 있음 (속도 향상)
//
bool cvTrimLocalMax32f(IplImage *src, IplImage *max, int dist)
{
	if (!cvSameSize(src, max)
		|| src->depth != 32 || max->depth != 8
		|| src->nChannels != 1 || max->nChannels != 1) return false;

	int w = cvWidth(src);
	int h = cvHeight(src);

	for (int y = dist; y<h - dist; y++) {
		unsigned char *c = cvGetPixelPtr(max, 0, y);
		for (int x = dist; x<w - dist; x++) {
			if (c[x] > 0) {
				CvRect rc = cvRect(x - dist, y - dist, dist * 2 + 1, dist * 2 + 1);
				IplImage win = cvGetWindow(max, rc);
				cvSetZero(&win);
				c[x] = 255;
			}
		}
	}

	return true;
}


int cvCalcArea(IplImage *lab, CvRect *rc, int gray)
{
	if (!cvIsGrayImage(lab)) return 0;
	IplImage win, *src;
	if (rc != NULL) {
		win = cvGetWindow(lab, *rc);
		src = &win;
	}
	else src = lab;

	int area = 0;
	for (int y = 0; y<cvHeight(src); y++) {
		unsigned char *c = cvGetPixelPtr(src, 0, y);
		for (int x = 0; x<cvWidth(src); x++) {
			if (c[x] == gray)
				area++;
		}
	}
	return area;
}
CvPoint2D32f cvMassCenter(IplImage *lab, CvRect *rc, int gray, int *pArea)
{
	if (!cvIsGrayImage(lab)) return cvPoint2D32f(0.0, 0.0);
	IplImage win, *src;
	if (rc != NULL) {
		win = cvGetWindow(lab, *rc);
		src = &win;
	}
	else src = lab;

	int area = 0, sumx = 0, sumy = 0;
	for (int y = 0; y<cvHeight(src); y++) {
		unsigned char *c = cvGetPixelPtr(src, 0, y);
		for (int x = 0; x<cvWidth(src); x++) {
			if (c[x] == gray) {
				sumx += x;
				sumy += y;
				area++;
			}
		}
	}
	if (pArea != NULL) *pArea = area;
	return cvPoint2D32f((float)sumx / area, (float)sumy / area);
}
