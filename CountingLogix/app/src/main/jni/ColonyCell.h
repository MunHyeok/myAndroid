//
// Created by CHK on 2016-03-22.
//

#ifndef ANDROID_COLONYCELL_H
#define ANDROID_COLONYCELL_H
#include "Img.h"

class CColonyCell
{
    CvPoint		m_center;		// 셀 중심
    float		m_radius;		// 셀 반경
    bool		m_bValid;		// Valid한 셀인가?
    bool		m_bFlag;		// 셀 Flag 선택 등에 사용

public:
    CColonyCell(void) :m_bFlag(false) {}
    ~CColonyCell(void)	{}

    bool	 IsSelected()	{ return m_bFlag; }
    CvPoint Center()		{ return m_center;}
    float&	 Radius()		{ return m_radius;}

    void Set(int x, int y, float radius, bool bValid=true, bool bFlag=false) { Set(cvPoint(x,y), radius, bValid, bFlag); }
    void Set(CvPoint cen, float radius, bool bValid=true, bool bFlag=false) {
        m_center = cen;
        m_radius = radius;
        m_bFlag	 = false;
        m_bValid = bValid;
    }
    void Select(CvPoint& pt, bool bMulti=false) {
        double dist = cvPointDist( pt, m_center );
        if( dist < 8. )
            m_bFlag = !m_bFlag;
        else if( bMulti == false )
            m_bFlag = false;
    }
    void Select(CvRect& rc, bool bMulti=false) {
        if( cvIsInside( rc, m_center ) )
            m_bFlag = true;
        else if( bMulti == false )
            m_bFlag = false;
    }
    void Draw(IplImage *dpy) {
        int		 lw = m_bFlag ? 3 : 1;
        CvScalar color = m_bValid ? CV_RGB(0,0,255) : CV_RGB(255,0,0);
        cvCircle(dpy, m_center, cvRound(m_radius), color, lw );
    }
};



#endif //ANDROID_COLONYCELL_H
