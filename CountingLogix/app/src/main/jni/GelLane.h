//
// Created by CHK on 2016-03-22.
//

#ifndef ANDROID_GELLANE_H
#define ANDROID_GELLANE_H
#include "cimUtils.h"
#include "GelBand.h"

const int GelMaxDim = 4096;		// 영상의 최대 길이 (가로 또는 세로)
const int GelMinLanes = 4;		// 영상의 최소 Lane의 수
const int GelMaxLanes = 256;	// 영상의 최대 Lane의 수
const int GelSegment = 12;		// 영상의 수직분할 수



class CGelLane
{
public:
    float		m_sx;
    float		m_ex;
    bool		m_bValid;
    bool		m_bSelected;

    IplImage	m_win;
    CvRect		m_rc;

    float		*m_hPrj;		// Lane 영역의 수평 (동적 할당은 괜찮음... 왜 그렇지?)
    int			m_nBands;
    float		*m_band;
    CGelBand	*m_gelBand;

    //------------------------------------------------------
    // Lane 추적을 위한 멤버들
    // 영상을 수직으로 8등분한 각 부분에 대한 중심값 저장
    //	- normal lane 이면 m_sx와 m_ex의 중앙값을 동일하게 저장
    //	- slanted lane이면 일단 정상부분의 중앙값을 해당 배열에 저장하고
    //		위쪽, 아랫쪽으로 추적하면서 각 tracking window에 동일한 처리를 하여
    //		중앙값을 계산하고 그 값을 m_center에 저장함
    //		==> 이것을 구현하면 될 것으로 보임
    //		==> 이에 따라 출력
    float		*m_center;			// 계산된 중심 (가로값은 검출된 레인의 수에 따라 다르기 때문에 동적할당)
    bool		*m_bTrue;		// 계산된 중심이 유효한 중심인지? 논문에 빨간색(true)/노란색(false) 중심에 해당함
    //float		m_center[12];		// 계산된 중심 (세로값은 8등분에 의해 자동으로 계산(블록 중심), 가로값은 추적에 의해 계산)
    //bool		m_bTrue[12];		// 계산된 중심이 유효한 중심인지? 논문에 빨간색(true)/노란색(false) 중심에 해당함

    //------------------------------------------------------

    CGelLane (void): m_nBands(0), m_bValid(false), m_bSelected(false) {
        m_hPrj = new float [GelMaxDim];
        m_band = new float [GelMaxBands];
        m_gelBand = new CGelBand [GelMaxBands];
        m_center = new float [GelMaxLanes];
        m_bTrue = new bool [GelMaxLanes];
    }
    ~CGelLane(void) {
        delete [] m_hPrj;
        delete [] m_band;
        delete [] m_gelBand;
    }

    bool	IsValid()	{ return m_bValid; }
    bool	IsSelected(){ return m_bSelected; }
    void	Set( float sx, float ex, bool bValid=false ) {
        m_sx	= sx;
        m_ex	= ex;
        m_bValid= bValid;
        m_bSelected = false;
    }
    // 영역이 설정된 이후 영상을 이용한 세부적 설정
    void Set( IplImage *src ) {
        if( src != NULL ) {
            m_rc = cvRect( cvRound(m_sx), 0, cvRound(m_ex-m_sx), cvHeight(src) );
            m_win = cvGetWindow( src, m_rc );
            cimProjH( &m_win, m_hPrj );
        }

    }

    bool IsInside( CvPoint& pt )	{ return cvIsInside(m_rc, pt); }
    void Toggle( CvPoint& pt )		{ if(IsInside(pt)) m_bValid = !m_bValid; }
    void Select( CvPoint pt, bool bMulti = false ) {
        if( IsInside(pt) ) m_bSelected = true;
        else if( bMulti == false ) m_bSelected = false;
    }
    void Move( bool bRight ) {
        if(IsSelected()) {
            m_sx += (bRight) ? 1.0f : -1.0f;
            m_ex += (bRight) ? 1.0f : -1.0f;
            m_rc.x += (bRight) ? 1 : -1;
        }
    }

    //========================================================================
    // Band Detection
    void FindBands() {
        m_nBands = CGelBand::FindAllBands( m_hPrj, m_rc.height, m_band );

        for( int i=0 ; i<m_nBands ; i++ ) {
            CGelBand& band = m_gelBand[i];

            int peaky		= cvRound(m_band[i]);
            band.m_peak		= m_band[i];	// bandY
            band.m_peakGray = m_hPrj[peaky];
            CalcBandInfo( i, band.m_minY, band.m_maxY, band.m_avgGray );
            band.m_width = band.m_maxY - band.m_minY + 1;
        }
    }

    void GetResult( int& nBands, float& laneX,
                    float* bandY, float* bandSize,
                    float* bandAvgBrightness, float* bandPeakBrightness )
    {
        nBands	= m_nBands;
        laneX	= m_rc.x + 0.5f * m_rc.width;

        for( int i=0 ; i<nBands ; i++ ) {
            CGelBand& band = m_gelBand[i];

            bandY[i]				= band.m_peak;
            bandPeakBrightness[i]	= band.m_peakGray;
            bandSize[i]				= (float)(band.m_width);
            bandAvgBrightness[i]	= band.m_avgGray;
        }
    }
    void CalcBandInfo( int i, int& miny, int& maxy, float &brightness ) {
        int si, ei, mi;
        si = (i==0) ? 0 : cvRound(m_band[i-1]);
        ei = (i==m_nBands-1) ? m_rc.height : cvRound(m_band[i+1]);
        mi = cvRound(m_band[i]);

        float minVal1, minVal2;
        cimFindMinMax( m_hPrj, si, mi, &minVal1, NULL );
        cimFindMinMax( m_hPrj, mi, ei, &minVal2, NULL );

        int wsi, wei;
        float minVal = cvMAX(minVal1, minVal2);
        float thVal = minVal + 0.5f*(m_hPrj[mi] - minVal);

        for( wsi=mi-1 ; wsi>si ; wsi-- )
            if( m_hPrj[wsi] < thVal ) break;
        for( wei=mi+1 ; wei<ei ; wei++ )
            if( m_hPrj[wei] < thVal ) break;

        miny = wsi;
        maxy = wei;
        int width = wei - wsi + 1;
        float sum = 0;
        for( int k=wsi ; k<=wei ; k++ )
            sum += m_hPrj[k];
        brightness = sum / width;
    }

    void Draw( IplImage *dpy ) {
        if( dpy != NULL ) {
            CvRect rc = m_rc;
            rc.height--;
            int thick = m_bSelected ? 2 : 1;

            if( IsValid() ){
                cvRectangle( dpy, rc, cvScalar(0,255,0), thick );
                for(int i=0 ; i<12 ; i++);
                //cvCircle( dpy, cvPoint((rc.x+rc.width/2), (((m_rc.height/12)*i)+((m_rc.height/12)*(i+1)))/2), 1, CV_RGB(0, 255, 0), 2 );
                // GelTool.cpp 의 Draw( )에서 m_center, m_bTrue 값 설정하면서 center점을 그려줌
                // 여기서 표시할지?? vs GelTool.cpp 의 Draw( )에서 표시할지??
                // 현재는 양쪽 모두에서 제대로 출력되게끔 전부 수정됨
            }
            else{
                cvRectangle( dpy, rc, cvScalar(0,0,150), thick );
                for(int i=0 ; i<12 ; i++);
                //cvCircle( dpy, cvPoint((rc.x+rc.width/2), (((m_rc.height/12)*i)+((m_rc.height/12)*(i+1)))/2), 1, CV_RGB(255, 255, 0), 2 );
            }

            /* // band를 그리는 부분
            for( int i=0 ; i<m_nBands ; i++ ) {
                cvLine( dpy, cvPoint(rc.x, cvRound(rc.y+m_band[i])),
                             cvPoint(rc.x+rc.width, cvRound(rc.y+m_band[i])),
                             cvScalar(255,0,0), 1);

                CvRect brc = cvRect( rc.x+1, cvRound(rc.y+m_gelBand[i].m_minY),
                                    rc.width-2, cvRound(m_gelBand[i].m_width) );
                cvRectangle( dpy, brc, cvScalar(0,0,200), 1 );
            }
            */
        }
    }

    //===============================================================
    // 초기 검출된 값으로 영상을 수직 N등분한 영역의 Center값 저장  : 복주
    // m_center, m_bTrue 값 설정
    //===============================================================
    void SetCenter ( IplImage *dpy, int i ) {
        CvRect rc = m_rc;
        rc.height--;

        if( IsValid() ){
            m_center[i] = rc.x+rc.width/2;
            m_bTrue[i] = true;
        }
        else{
            m_center[i] = rc.x+rc.width/2;
            m_bTrue[i] = false;
        }
    }
};


#endif //ANDROID_GELLANE_H
