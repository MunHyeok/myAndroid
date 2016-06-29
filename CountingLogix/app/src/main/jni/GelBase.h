//
// Created by CHK on 2016-03-22.
//

#ifndef ANDROID_GELBASE_H
#define ANDROID_GELBASE_H
#include "Img.h"
#include "GelLane.h"

class CGelBase
{
protected:
public:
    char		m_name[200];	// 모델 이름
    IplImage*	m_org;			// 원 영상 (whole image) : 외부에서 제공
    CImg		m_dpy;			// 원 영상에 처리 결과를 기록한 출력용 영상 (whole image)
    CvRect		m_rcAnal;		// 원 영상에서 분석할  영상 영역

    IplImage	m_src;			// 분석용 영상 (관심영역 영상)
    IplImage	m_win;			// 분석용 영상의 출력용

//	bool		m_bBG;			// 영상의 배경이 밝은 경우 ==> true (보통 false)

public:
    CGelBase(void) : m_org(NULL) { SetName("IntelliGel"); }
    virtual ~CGelBase(void) {}

    bool		IsValid()	{ return m_org != NULL; }
    IplImage*	GetOrg()	{ return m_org; }
    IplImage*	GetDisplay(){ return m_dpy.GetImage(); }
    IplImage*	GetImage()	{ return IsValid() ? &m_src : NULL; }
    IplImage*	GetDpyWin()	{ return IsValid() ? &m_win : NULL; }

    void SetName(const char *name)	{ strcpy( m_name, name ); }
    char*		GetName()	{ return m_name; }

    //===============================================================
    // 입력 영상 설정.
    bool SetImage( IplImage *org, CvRect *rc = NULL ) {
        if( org != NULL ) {
            m_org = org;				// 원 영상
            m_dpy.Copy( org );			// 출력 영상
            SetRect( rc );
            return true;
        }
        return false;
    }
    void SetRect( CvRect *rc ) {
        if( IsValid() ) {
            m_rcAnal= (rc==NULL) ? cvRect(m_org) : *rc;
            m_src = cvGetWindow( m_org, m_rcAnal );
            m_win = cvGetWindow( m_dpy.GetImage(), m_rcAnal );
        }
    }

    virtual void Draw( ) {}
};


#endif //ANDROID_GELBASE_H
