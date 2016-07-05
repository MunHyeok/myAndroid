//=============================================================================
// cimProj.cpp
//	- image projection/histogram �� 1���� �迭�� ������ ó���� ���� �Լ���
//	- 1���� �迭�� ���������� ȿ������ ���̱� ���� float array�� ó��
//
//
//=============================================================================
#include "cimUtils.h"


//=============================================================================
//	������ ����/���� ���������� ���� �Լ���
//=============================================================================

//=============================================================================
// Horizontal / Vertical Projection Profile �� Profile�� �̺� ���
//	- ������ ROI �� ��쿡�� ������
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

// ����/���� line�� �� ȭ���� ���� ���� ���� ��ȯ�ϴ� �Լ�
//	- ���� channel�� ��� ��� ä���� ȭ�Ұ��� ����.
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

// ������ ���� �������� �ڵ�
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

// ����/���� 1���� ���������� ȭ��(����)���� ����ϴ� �Լ���
// bOverlap�̸� ȭ���� Clear ���� ����
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
	if (bOverlap == false)			// overlap�� �ƴϸ� ���� ��� ������ 0���� ����
		cvSetZero(img);

	float scale = maxv / (max - min);

	for (int y = 0; y<maxv; y++) {
		for (int x = 0; x<bins; x++) {
			int val = maxv - cvRound((prj[x] - min) * scale);
			if (y >= val) {
				unsigned char *pxl = cvGetPixelPtr(img, x, y);

				// overlap�̸� �Ķ��� ä�θ�, �ƴϸ� ��ü ä�� �� ����
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
	if (bOverlap == false)			// overlap�� �ƴϸ� ���� ��� ������ 0���� ����
		cvSetZero(img);

	float scale = maxv / (max - min);

	for (int y = 0; y<bins; y++) {
		int val = cvRound((prj[y] - min) * scale);
		int sx = (toLeft) ? maxv - val : 0;
		int ex = (toLeft) ? maxv : val;

		for (int x = sx; x<ex; x++) {
			unsigned char *pxl = cvGetPixelPtr(img, x, y);

			// overlap�̸� �Ķ��� ä�θ�, �ƴϸ� ��ü ä�� �� ����
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


// ���� �������Ͽ��� i ���Ŀ� ó�� ������ rising/falling edge ��ġ�� ��ȯ
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
//	�������� ���������̳� ������׷��� �м��� ���� �Լ���
//=============================================================================

// �迭���� �ִ밪�� �ּҰ��� ã�� ��ȯ (Projection Profile���� ����� ���)
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

// �迭���� �ִ밪�� �ּҰ��� ã�� ��ȯ (���󿡼� ����� ���)
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

// �迭���� �ִ밪/�ּҰ��� ��ġ�� ��ȯ. �ʿ�� sub-pixel ���� ��ġ�� ��ȯ
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

// �迭�� ������ ����ȭ (�ִ밪�� maxVal�� ó��)
void	cimNormalizeArray(float *prj, int len, float maxVal)
{
	float	posi = cimFindMaxPosi(prj, len, false);

	if (posi >= 0.0f) {
		float scale = maxVal / prj[cvRound(posi)];
		for (int i = 0; i<len; i++)
			prj[i] = prj[i] * scale;
	}
}

// �迭�� ��հ� ǥ�� ������ ����
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

// �迭���� mini/maxi ��ġ�� winsize ũ�� ������ �ּҰ�/�ִ밪���� �Ǵ�
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
		if (prj[i] > maxv) return false;	// �ϴ� �ִ��� ���� �̵��� ��
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
// ���� �������Ͽ��� i������ Falling Edge��ġ�� ����ϴ� �Լ�
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
// ���� �������Ͽ��� i������ Falling/Rising Edge��ġ�� ����ϴ� �Լ�
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
// ���� ���������� ó������ ���� ���� ��Ÿ���� Falling Edge��ġ�� ����ϴ� �Լ�
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
// ���������� ����� �м� �Լ�
//	- min, max  : ���������� �ּҰ�, �ִ밪
//	- mean	    : ��հ�
//	- dev	    : �л�
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


// ��.... ����� �ߴµ�... �� ������ ���� ���� ...
// �ϴ��� ������� ���� ...
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
// ���� �������Ͽ��� i������ Local Peak Value��ġ�� ����ϴ� �Լ�
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