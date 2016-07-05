#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include "cimUtils.h"
//#define IMEDIA_FILENAME	"ykchoi@koreatech.ac.kr"
#define IMEDIA_FILENAME	"None"

class CImg
{
protected:
	char			m_name[200];	// 화면에 표시할 이름
	IplImage		*m_image;		// 실제 영상 -> 플랫폼에 따라 변경

public:

	//=========================================================
	// 생성 / 소멸 / 다양한 복사연산
	CImg(void)		{ m_image = NULL; Reset(); }
	~CImg(void)		{ Reset(); }
	CImg(CImg& img)	{ m_image = NULL; Clone(&img); }
	void Reset()	{ m_name[0] = '\0'; cvReleaseImage(&m_image); }

	void Create(CvSize sz, int depth, int nch, const char *filename = IMEDIA_FILENAME) {
		cvCreateSmart(sz, depth, nch, &m_image);
		strcpy(m_name, filename);
	}
	void Copy(IplImage *src, const char *name = IMEDIA_FILENAME) {
		cvCopySmart(src, &m_image);
		strcpy(m_name, name);
	}
	void Copy(CImg *src)	{ if (src != NULL && src->IsValid()) Copy(src->m_image); }
	void Clone(CImg *src) { if (src != NULL && src->IsValid()) Copy(src->m_image, src->GetName()); }
	bool Copy(CImg &src, CvRect rcSrc, CvRect rcDst) {
		if (!IsValid() || !src.IsValid() || src.Channel() != Channel()) return false;

		Copy(src.GetImage(), rcSrc, rcDst);
		return true;
	}
	void Copy(IplImage *src, CvRect rcSrc, CvRect rcDst) {
		IplImage srcImg = cvGetWindow(src, rcSrc);
		IplImage dstImg = cvGetWindow(GetImage(), rcDst);
		cvCopy(&srcImg, &dstImg);
	}
	void CopyReduce(IplImage *src, int scale = 2) {
		if (src != NULL) {
			CvRect rc = cvRect(src);
			Create(cvSize(rc.width / scale, rc.height / scale), src->depth, src->nChannels, "ReducedCopy");
			cvResize(src, m_image);
		}
	}
	void CopyToGray(IplImage *src, CvRect *r = NULL) {
		if (cvIsRGBImage(src)) {
			if (r == NULL) {
				CvRect rc = cvRect(src);
				Create(cvSize(rc.width, rc.height), 8, 1, "CopyGray");
				cvCvtColor(src, m_image, CV_RGB2GRAY); // 컬러를 흑백으로 변환
			}
			else {
				IplImage win = cvGetWindow(src, *r);
				CvRect rc = cvRect(&win);
				Create(cvSize(rc.width, rc.height), 8, 1, "CopyGray");
				cvCvtColor(&win, m_image, CV_RGB2GRAY); // 컬러를 흑백으로 변환
			}
		}
	}


	//=========================================================
	// 간단한 정보 추출
	// Basic Image Processing methods
	int				Width()			{ return (IsValid()) ? cvWidth(m_image) : 0; }
	int				Height()		{ return (IsValid()) ? cvHeight(m_image) : 0; }
	int				Depth()			{ return (IsValid()) ? m_image->depth : 0; }
	int				Channel()		{ return (IsValid()) ? m_image->nChannels : 0; }
	CvSize			Size()			{ return (IsValid()) ? cvSize(Width(), Height()) : cvSize(0, 0); }
	CvRect			Rect()			{ return (IsValid()) ? cvRect(m_image) : cvRect(0, 0, 0, 0); }
	IplImage*		GetImage()		{ return m_image; }
	char*			GetName()		{ return m_name; }
	bool			IsSameSize(CImg *src)		{ return cvSameSize(Size(), src->Size()); }
	unsigned char*	GetPixelPtr(CvPoint pt)	{ return cvGetPixelPtr(m_image, pt); }
	unsigned char&	GetPixel(CvPoint pt)	{ return *GetPixelPtr(pt); }
	unsigned char&	GetPixel(int x, int y)	{ return GetPixel(cvPoint(x, y)); }
	bool			IsBGPixel(CvPoint pt)	{ return GetPixel(pt) == 0; }
	bool			IsValid()				{ return m_image != NULL; }
	const char*		GetImageInfo();
	const char*		GetShortName();

	//=========================================================
	// File IO / Display
	bool LoadSrcImage(const char *filename) {
		Reset();
		strcpy(m_name, filename);
		m_image = cvLoadImage(m_name);
		return m_image != NULL;
	}
	bool LoadGrayImage(const char *filename) {
		if (LoadSrcImage(filename) == false)
			return false;

		return ConvertToGrayImage();
	}
	void Store(char *filename) {
		cvSaveImage(filename, GetImage());
	}

	bool ConvertToGrayImage() {
		if (cvIsRGBImage(m_image)) {
			IplImage *image = m_image;
			m_image = cvCreateImage(cvGetSize(image), 8, 1);
			cvCvtColor(image, m_image, CV_RGB2GRAY); // 컬러를 흑백으로 변환
			cvReleaseImage(&image);
			return true;
		}
		else return false;
	}
	void Show();
	bool Draw(CvPoint pt, const char *str);

};


extern const char*	cvGetDirName(const char *path);
extern const char*	cvGetFileName(const char *path);
