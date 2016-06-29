//
// Created by CHK on 2016-03-22.
//

#ifndef ANDROID_COLONYTOOL_H
#define ANDROID_COLONYTOOL_H
#include "GelBase.h"
#include "Blob.h"
#include "ColonyCell.h"
const int COL_MAX_CELLS = 10000;

class CColonyTool : public CGelBase
{
    CImg			m_gray;			// 원 영상의 Gray 영상
    CImg			m_bin;			// 이진화 영상
    CImg			m_dst;			// Distance Transform 영상 (32bit)
    CImg			m_dish;			// petri dish 영역 mask 영상
    CImg			m_lmax;			// local maximum 영상
    CvPoint2D32f	m_dishCenter;	// petri dish의 중심
    float			m_dishRadius;	// petri dish의 반지름

    CvMemStorage*	m_storage;
    CColonyCell*	m_cell;
    int				m_nCells;
    int				m_th;			// 이진화 문턱치

public:
    CColonyTool(void) {
        m_storage = cvCreateMemStorage();
        m_cell = new CColonyCell[COL_MAX_CELLS];
        m_nCells = 0;
    }
    ~CColonyTool(void) {
        cvReleaseMemStorage(&m_storage );
        delete [] m_cell;
    }
    int		GetTh()	{ return m_th; }

    bool SetImage( IplImage *org, CvRect *rc = NULL ) {
        if( CGelBase::SetImage( org, rc ) ) {
            m_gray.CopyToGray( GetImage() );
            m_bin.Clone( &m_gray );
            m_dish.Clone( &m_gray );
            m_lmax.Clone( &m_gray );
            cvSet( m_dish.GetImage(), cvScalar(255, 255, 255) );

            m_dst.Create( cvSize(m_gray.Width(), m_gray.Height()), 32, 1);
            m_dishRadius = 0.0f;
            return true;
        }
        return false;
    }
    int	CountColony( int offset = 0, bool bInit = false );
    int	RegisterCells( IplImage *max );

    void CalcDish( CvPoint2D32f& p1, CvPoint2D32f& p2, CvPoint2D32f& p3 ) {
        if( cvCirclePPP( p1, p2, p3, m_dishCenter, m_dishRadius ) ) {
            cvSetZero( m_dish.GetImage() );
            cvCircle( m_dish.GetImage(), cvPoint(m_dishCenter), cvRound(m_dishRadius),
                      cvScalar(255, 255, 255), -5 );
            CountColony( );
        }
    }
    //================================================================
    // LBUTTON         ==> 단일 Cell 선택/취소
    // LBUTTON + Shift ==> 다중 Cell 선택/취소
    void Select( CvPoint pt, bool bMulti=false ) {
        for( int i=0 ; i<m_nCells ; i++ )
            m_cell[i].Select(pt, bMulti);
    }
    //================================================================
    // 사각영역         ==> 단일 Cell 선택/취소
    // 사각영역 + Shift ==> 다중 Cell 선택/취소
    void Select( CvRect& rc, bool bMulti=false ) {
        for( int i=0 ; i<m_nCells ; i++ )
            m_cell[i].Select(rc, bMulti);
    }

    //void FindDish();
    //void AutoCount( );
    void FindBlobs( );

    void MergeColony( ) {
        CvPoint	pt = cvPoint(0,0);
        float	radius = 0.f;
        int		count = 0;
        for( int i=0 ; i<m_nCells ; i++ ) {
            if( m_cell[i].IsSelected() ) {
                pt = cvPointAdd(pt, m_cell[i].Center());
                radius += m_cell[i].Radius();
                count++;
                m_cell[i] = m_cell[m_nCells-1];
                m_nCells--;
                i--;
            }
        }
        if( count > 0 ) {
            pt.x /= count;
            pt.y /= count;
            m_cell[m_nCells++].Set( pt, radius/count, true, true );
        }
    }
    void DeleteColony( ) {
        for( int i=0 ; i<m_nCells ; i++ ) {
            if( m_cell[i].IsSelected() ) {
                m_cell[i] = m_cell[m_nCells-1];
                m_nCells--;
                i--;
            }
        }
    }
    void Draw( ) {
        if( m_org != NULL && m_dpy.IsValid() )
            m_dpy.Copy( m_org );			// 원 영상을 출력 영상에 다시 복사

        IplImage *rgb = GetDisplay();
        if( rgb == NULL ) return;

        // Petri Dish 영역을 표시
        if( m_dishRadius > 100 )
            cvCircle(rgb, cvPoint(m_dishCenter), cvRound(m_dishRadius), CV_RGB(255,0,0), 5 );

        // 찾아진 모든 셀들을 화면에 표시
        for( int i=0 ; i<m_nCells ; i++ )
            m_cell[i].Draw( rgb );

        // 전체적인 통계 데이터를 화면 좌상단에 출력
        //char* msg[200];
        //CvScalar color = CV_RGB(255,255,0);
        //sprintf( msg, "Number of Cells = %d", m_nCells );
        //cvDrawText( rgb, msg, cvPoint( 10, 40 ), 5, color , 1.0, 1 );
    }
};


#endif //ANDROID_COLONYTOOL_H
