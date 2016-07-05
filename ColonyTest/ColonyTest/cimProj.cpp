//=============================================================================
// cimProj.cpp
//	- image projection/histogram 및 1차원 배열의 간단한 처리를 위한 함수들
//	- 1차원 배열은 간단하지만 효율성을 높이기 위해 float array로 처리
//
//
//=============================================================================
#include "cimUtils.h"


//=============================================================================
//	영상의 수직/수평 프로젝션을 위한 함수들
//=============================================================================

//=============================================================================
// Horizontal / Vertical Projection Profile 및 Profile의 미분 계산
//	- 영상이 ROI 인 경우에도 동작함
//
//	- cimProjH()
//	- cimProjV()
//	- cimEdge ()
//
float* cimProjH(IplImage *img, float *prj)
{
	static float arr[4096];

	if (img == NULL || img->depth != 8) return NULL;
	if (prj == NULL) prj = arr;

	CvRect	rc = cvRect(img);
	int		npix = rc.width * img->nChannels;
	for (int i = 0; i<rc.height; i++) {
		prj[i] = (float)cimSumHLine(img, 0, i, rc.width) / npix;
	}
	return prj;
}
float* cimProjV(IplImage *img, float *prj)
{
	static float arr[4096];

	if (img == NULL || img->depth != 8) return NULL;
	if (prj == NULL) prj = arr;

	CvRect	rc = cvRect(img);
	int		npix = rc.height* img->nChannels;
	for (int i = 0; i<rc.width; i++) {
		prj[i] = (float)cimSumVLine(img, i, 0, rc.height) / npix;
	}
	return prj;
}
float* cimEdge(float *prj, int len, float *edge)
{
	static float arr[4096];
	if (prj == NULL || len <= 3) return NULL;
	if (edge == NULL) edge = arr;

	edge[0] = edge[len - 1] = 0.0f;
	for (int i = 1; i<len - 1; i++) {
		edge[i] = 0.5f * (prj[i + 1] - prj[i - 1]);
	}
	return edge;
}
float* cimEdge2(float *p, int len, float *e)
{
	static float arr[4096];
	if (p == NULL || len< 5) return NULL;
	if (e == NULL) e = arr;

	e[0] = e[1] = e[len - 2] = e[len - 1] = 0.0f;
	for (int i = 2; i<len - 2; i++) {
		e[i] = 0.25f * (p[i + 2] + p[i + 1] - p[i - 1] - p[i - 2]);
	}
	return e;
}

// 수직/수평 line의 각 화소의 값의 합을 구해 반환하는 함수
//	- 다중 channel인 경우 모든 채널의 화소값을 합함.
int cimSumHLine(IplImage *src, int x, int y, int width)
{
	unsigned char *ptr = cvGetPixelPtr(src, x, y);
	int sum = 0;
	for (int i = 0; i<width*src->nChannels; i++, ptr++)
		sum += *ptr;

	return sum;
}
int cimSumVLine(IplImage *src, int x, int y, int height)
{
	int	n = src->nChannels;
	int sum = 0;
	for (int i = 0; i<height; i++, y++) {
		unsigned char *ptr = cvGetPixelPtr(src, x, y);
		for (int c = 0; c<n; c++, ptr++)
			sum += *ptr++;
	}
	return sum;
}

// 선택적 수직 프로젝션 코드
float* cimProjV(IplImage *img, float *prj, float *h, float th)
{
	static float arr[4096];

	if (img == NULL || h == NULL || img->depth != 8) return NULL;
	if (prj == NULL) prj = arr;

	CvRect	rc = cvRect(img);
	int		nch = img->nChannels;
	int		nSum = 0;

	for (int x = 0; x<rc.width; x++) prj[x] = 0.0f;

	for (int y = 0; y<rc.height; y++) {
		if (h[y] >= th) {
			nSum++;
			unsigned char *pxl = cvGetPixelPtr(img, 0, y);
			for (int x = 0; x<rc.width; x++) {
				for (int k = 0; k<nch; k++, pxl++) prj[x] += *pxl;
			}
		}
	}
	if (nSum > 0)
	for (int x = 0; x<rc.width; x++) prj[x] /= (nSum * nch);

	return prj;
}

// 수직/수평 1차원 프로파일을 화면(영상)으로 출력하는 함수들
// bOverlap이면 화면을 Clear 하지 않음
void	cimDispNormV(float *prj, IplImage *img, bool bOverlap, bool toTop)
{
	if (prj != NULL && img != NULL) {
		float min, max;
		cimFindMinMax(prj, cvWidth(img), min, max);
		cimDispHistV(prj, img, min, max, bOverlap, toTop);
	}
}
void	cimDispNormH(float *prj, IplImage *img, bool bOverlap, bool toLeft)
{
	if (prj != NULL && img != NULL) {
		float min, max;
		cimFindMinMax(prj, cvHeight(img), min, max);
		cimDispHistH(prj, img, min, max, bOverlap, toLeft);
	}
}
void	cimDispHistV(float *prj, IplImage *img, float min, float max, bool bOverlap, bool toTop)
{
	if (prj == NULL || img == NULL || min >= max) return;
	int bins = cvWidth(img);
	int maxv = cvHeight(img);
	if (bOverlap == false)			// overlap이 아니면 먼저 출력 영상을 0으로 지움
		cvSetZero(img);

	float scale = maxv / (max - min);

	for (int y = 0; y<maxv; y++) {
		for (int x = 0; x<bins; x++) {
			int val = maxv - cvRound((prj[x] - min) * scale);
			if (y >= val) {
				unsigned char *pxl = cvGetPixelPtr(img, x, y);

				// overlap이면 파란색 채널만, 아니면 전체 채널 값 변경
				if (bOverlap) *pxl = 255;
				else for (int i = 0; i<img->nChannels; i++, pxl++) *pxl = 255;
			}
		}
	}
}
void	cimDispHistH(float *prj, IplImage *img, float min, float max, bool bOverlap, bool toLeft)
{
	if (prj == NULL || img == NULL || min >= max) return;
	int bins = cvHeight(img);		// img->width or img->roi->width
	int maxv = cvWidth(img);
	if (bOverlap == false)			// overlap이 아니면 먼저 출력 영상을 0으로 지움
		cvSetZero(img);

	float scale = maxv / (max - min);

	for (int y = 0; y<bins; y++) {
		int val = cvRound((prj[y] - min) * scale);
		int sx = (toLeft) ? maxv - val : 0;
		int ex = (toLeft) ? maxv : val;

		for (int x = sx; x<ex; x++) {
			unsigned char *pxl = cvGetPixelPtr(img, x, y);

			// overlap이면 파란색 채널만, 아니면 전체 채널 값 변경
			if (bOverlap) {
				if (img->nChannels == 1) *pxl = 255;
				else					  *(pxl + 1) = 255;
			}
			else for (int i = 0; i<img->nChannels; i++, pxl++) *pxl = 255;
		}
	}
}


float* cimFindRisingPosi(float *e, int len, int* pcnt, float mag)
{
	static float posi[4096];
	int count = 0;
	for (int i = 0; i<len; i++)
	if (cimIsRising(e, i, mag))
		posi[count++] = cimSubPixelPosi(e, i);

	*pcnt = count;
	return posi;
}
float* cimFindFallingPosi(float *e, int len, int* pcnt, float mag)
{
	static float posi[4096];
	int count = 0;
	for (int i = 0; i<len; i++)
	if (cimIsFalling(e, i, mag))
		posi[count++] = cimSubPixelPosi(e, i);

	*pcnt = count;
	return posi;
}


// 에지 프로파일에서 i 이후에 처음 나오는 rising/falling edge 위치를 반환
float cimNextPeak(float *e, int i, int w, float mag)
{
	for (; i<w; i++) {
		if (cimIsFalling(e, i, mag)
			|| cimIsRising(e, i, mag)) {
			return cimSubPixelPosi(e, i);
		}
	}
	return (float)w;
}

//
float cimNextRise(float *e, int i, int w, float mag)
{
	static int posi[4096];
	int count = 0;

	for (; i<w; i++) {
		if (count>0 && cimIsFalling(e, i, mag)) break;

		if (cimIsRising(e, i, mag)) {
			posi[count++] = i;
		}
	}
	if (count > 0) {
		int maxi = 0;
		for (int i = 1; i<count; i++)
		if (e[posi[maxi]] <= e[posi[i]]) maxi = i;

		return cimSubPixelPosi(e, posi[maxi]);
	}
	return (float)w;
}
float cimNextFall(float *e, int i, int w, float mag)
{
	static int posi[4096];
	int count = 0;

	for (; i<w; i++) {
		if (count>0 && cimIsRising(e, i, mag)) break;

		if (cimIsFalling(e, i, mag)) {
			posi[count++] = i;
		}
	}
	if (count > 0) {
		int mini = 0;
		for (int i = 1; i<count; i++)
		if (e[posi[mini]] >= e[posi[i]]) mini = i;

		return cimSubPixelPosi(e, posi[mini]);
	}
	return (float)w;
}

/*
float* cimMovingAvg( float *prj, int len, float *avg, int w )
{
static float arr[4096];
if( prj == NULL || len<=3 ) return NULL;
if( avg== NULL ) edge = arr;

float sum = 0.0f;

for( int i=0; i<2*w+1 ; i++ ) sum += prj[i];

for( int i=w ; i<len-w ; i++ ) {
avg[i] = sum / (2*w+1);
sum += prj[i+w] - prj[i-w];
}
return edge;
}
*/

//=============================================================================
//	프로젝션 프로파일이나 히스토그램의 분석을 위한 함수들
//=============================================================================

// 배열에서 최대값과 최소값을 찾아 반환 (Projection Profile에서 검출시 사용)
void	cimFindMinMax(float *hist, int sx, int ex, float* pminv, float* pmaxv)
{
	if (hist != NULL && sx >= 0 && sx<ex) {
		float minv, maxv;
		minv = maxv = hist[sx];
		for (int i = sx + 1; i<ex; i++) {
			if (minv > hist[i]) minv = hist[i];
			if (maxv < hist[i]) maxv = hist[i];
		}
		if (pminv != NULL) *pminv = minv;
		if (pmaxv != NULL) *pmaxv = maxv;
	}
}

// 배열에서 최대값과 최소값을 찾아 반환 (영상에서 검출시 사용)
void	cimFindMinMax(unsigned char *img, int sx, int ex, int &minv, int &maxv)
{
	if (img != NULL && sx >= 0 && sx<ex) {
		minv = maxv = img[sx];
		for (int i = sx + 1; i<ex; i++) {
			if (minv > img[i]) minv = img[i];
			if (maxv < img[i]) maxv = img[i];
		}
	}
}

// 배열에서 최대값/최소값의 위치를 반환. 필요시 sub-pixel 연산 위치를 반환
float	cimFindMaxPosi(float *prj, int si, int ei, bool bSubPixel)
{
	if (prj != NULL && si >= 0 && si<ei) {
		int	  maxi = si;
		for (int i = si + 1; i<ei; i++)
		if (prj[i] > prj[maxi]) maxi = i;

		return bSubPixel ? cimSubPixelPosi(prj, maxi) : (float)maxi;
	}
	else return -1.0;
}
float	cimFindMinPosi(float *prj, int si, int ei, bool bSubPixel)
{
	if (prj != NULL && si >= 0 && si<ei) {
		int	  mini = si;
		for (int i = si + 1; i<ei; i++)
		if (prj[i] < prj[mini]) mini = i;

		return bSubPixel ? cimSubPixelPosi(prj, mini) : (float)mini;
	}
	else return -1.0;
}

// 배열의 내용을 정규화 (최대값을 maxVal로 처리)
void	cimNormalizeArray(float *prj, int len, float maxVal)
{
	float	posi = cimFindMaxPosi(prj, len, false);

	if (posi >= 0.0f) {
		float scale = maxVal / prj[cvRound(posi)];
		for (int i = 0; i<len; i++)
			prj[i] = prj[i] * scale;
	}
}

// 배열의 평균과 표준 편차를 구함
void	cimCalcAvgSdv(float*prj, int len, float& mean, float& stdDev)
{
	if (prj != NULL && len > 0) {

		float sum = 0.0f;
		for (int i = 0; i<len; i++)
			sum += prj[i];
		mean = sum / len;

		sum = 0.0f;
		for (int i = 0; i<len; i++)
			sum += (mean - prj[i])*(mean - prj[i]);
		stdDev = (float)sqrt(sum / len);
	}
}

// 배열에서 mini/maxi 위치가 winsize 크기 내에서 최소값/최대값인지 판단
bool	cimIsLocalMin(float *prj, int len, int mini, int winsize)
{
	if (prj != NULL && mini >= 0 && mini<len) {
		int si = (mini>winsize) ? mini - winsize : 0;
		int ei = (mini + winsize < len) ? mini + winsize : len;

		float minv = prj[mini];
		for (int i = si + 1; i<ei; i++)
		if (prj[i] < minv) return false;
		return true;
	}
	return false;
}
bool	cimIsLocalMax(float *prj, int len, int maxi, int winsize)
{
	if (prj != NULL && maxi >= 0 && maxi<len) {
		int si = (maxi>winsize) ? maxi - winsize : 0;
		int ei = (maxi + winsize < len) ? maxi + winsize : len;

		float maxv = prj[maxi];
		for (int i = si + 1; i<ei; i++)
			//			if( prj[i] >= maxv && i != maxi ) return false;
		if (prj[i] > maxv) return false;	// 일단 최대한 많이 뽑도록 함
		return true;
	}
	return false;
}

float	cimLocalMaxDiff(float *prj, int len, int maxi, int winsize)
{
	if (prj != NULL && maxi >= 0 && maxi<len) {
		int si = (maxi>winsize) ? maxi - winsize : 0;
		int ei = (maxi + winsize < len) ? maxi + winsize : len;

		float maxd = 0;
		float maxv = prj[maxi];
		for (int i = si + 1; i<ei; i++) {
			if (prj[i] >= maxv && i != maxi) return -1.0f;
			float diff = maxv - prj[i];
			if (diff > maxd) maxd = diff;
		}
		return maxd;
	}
	return false;
}






float* cimMovingAvg(float *p, int len, int hw, float *q)
{
	static float sarr[4096];
	float *arr = (q == NULL) ? sarr : q;

	if (p == NULL || len<hw * 2) return NULL;

	float sum = 0.0f;
	for (int i = 0; i<hw; i++) sum += p[i];

	for (int i = 0; i<hw; i++) {
		sum += p[i + hw];
		arr[i] = sum / (hw + i + 1);
	}
	for (int i = hw; i<len - hw; i++) {
		sum += (p[i + hw] - p[i - hw]);
		arr[i] = sum / (hw + hw + 1);
	}
	for (int i = len - hw; i<len; i++) {
		sum -= p[i - hw];
		arr[i] = sum / (hw + hw + 1);
	}
	return arr;
}


#ifdef XXX

//--------------------------------------------------------------------------------
//
// 현재 프로파일에서 i다음의 Falling Edge위치를 계산하는 함수
//
//--------------------------------------------------------------------------------
float cpjNextFalling(float *e, int i, int w, float minDiff)
{
	float	posi = 0;

	for (; i<w; i++) {
		if (cpjIsFallingEdge(e, i, minDiff)) {
			posi = cpjSubpixelPosi(e, i);
			return posi;
		}
	}

	return posi;
}

//--------------------------------------------------------------------------------
//
// 현재 프로파일에서 i다음의 Falling/Rising Edge위치를 계산하는 함수
//
//--------------------------------------------------------------------------------
float cpjNextPeak(float *e, int i, int w, float minDiff)
{
	for (; i<w; i++) {
		if (cpjIsFallingEdge(e, i, minDiff)
			|| cpjIsRisingEdge(e, i, minDiff)) {
			return cpjSubpixelPosi(e, i);
		}
	}

	return (float)w;
}


//--------------------------------------------------------------------------------
//
// 현재 프로파일의 처음부터 가장 먼저 나타나는 Falling Edge위치를 계산하는 함수
//
//--------------------------------------------------------------------------------
float cpjFirstFalling(float *e, int w, float minDiff)
{
	int		i, nhw = 2, npeak = 0;
	float	posi = -1.0f, val = 0.0f;

	i = nhw;
	posi = cpjNextFalling(e, i, w, minDiff);

	return posi;
}




float cpjFirstW2B(unsigned char *buf, int w, int gTh)
{
	for (int i = 0; i<w; i++)
	if (buf[i] > gTh + 3) break;

	for (; i<w; i++)
	if (buf[i] < gTh - 3) break;

	if (i == w) return 0.f;
	else return i - 0.5f;
}



//================================================================================
//--------------------------------------------------------------------------------
//
// 프로파일의 통계적 분석 함수
//	- min, max  : 프로파일의 최소값, 최대값
//	- mean	    : 평균값
//	- dev	    : 분산
//
//--------------------------------------------------------------------------------
void cpjStatistic(float *data, int w, float* pmean, float* pdev, float* pmin, float* pmax)
{
	if (data == NULL || w <= 0) return;

	// 1) Mean
	float sum = 0.;
	for (int i = 0; i<w; i++) {
		sum += data[i];
	}
	float mean = sum / w;
	if (pmean != NULL) *pmean = mean;


	// 2) Deviation
	if (pdev != NULL) {
		sum = 0;
		for (i = 0; i<w; i++) {
			float dev = mean - data[i];
			sum += (dev * dev);
		}
		*pdev = sum / w;
	}

	if (pmin != NULL || pmax != NULL) {
		float min = data[0];
		float max = data[0];
		for (i = 1; i<w; i++) {
			if (data[i] < min) min = data[i];
			if (data[i] > max) max = data[i];
		}
		if (pmax != NULL) *pmax = max;
		if (pmin != NULL) *pmin = min;
	}
}

void cpjMinMax(float *data, int w, float* pmin, float* pmax)
{
	if (data == NULL || w <= 0) return;

	float min = data[0];
	float max = data[0];
	for (int i = 1; i<w; i++) {
		if (data[i] < min) min = data[i];
		if (data[i] > max) max = data[i];
	}
	if (pmax != NULL) *pmax = max;
	if (pmin != NULL) *pmin = min;
}
float cpjMaxDeviation(float *data, int w)
{
	float min = 0.f;
	float max = 0.f;
	cpjMinMax(data, w, &min, &max);
	return max - min;
}


// 음.... 만들긴 했는데... 좀 문제가 있을 수도 ...
// 일단은 사용하지 말고 ...
void cpjRefProfile(float *data, float *ref, int len, int winsize)
{
	if (data == NULL || ref == NULL || len < winsize) return;

	float	sum = 0.0f;
	int		i, id;
	int		half = winsize / 2;

	for (i = 0; i<winsize; i++) sum += data[i];

	for (id = 0; id<half; id++) ref[id] = sum / winsize;

	for (; i<len; i++, id++) {
		sum += (data[i] - data[i - winsize]);
		ref[id] = sum / winsize;
	}

	for (; id<len; id++) ref[id] = sum / winsize;
}


//================================================================================
//--------------------------------------------------------------------------------
//
// 현재 프로파일에서 i다음의 Local Peak Value위치를 계산하는 함수
//
//--------------------------------------------------------------------------------
inline bool isLocalMax(float *e, int i)
{
	return(e[i] >= e[i - 2]
		&& e[i] >= e[i - 1]
		&& e[i] >  e[i + 1]
		&& e[i] >  e[i + 2]
		);
}
inline bool isLocalMin(float *e, int i)
{
	return(e[i] <= e[i - 2]
		&& e[i] <= e[i - 1]
		&& e[i] <  e[i + 1]
		&& e[i] <  e[i + 2]
		);
}
int cpjNextLocalMaxPosi(float *e, int i, int w)
{
	for (; i<w - 3; i++) {
		if (isLocalMax(e, i)
			&& cpjMaxDeviation(e + i - 2, 5) > 6) {
			return i;
		}
	}
	return w;
}
int cpjNextLocalMinPosi(float *e, int i, int w)
{
	for (; i<w - 3; i++) {
		if (isLocalMin(e, i)) {
			return i;
		}
	}
	return w;
}

#endif