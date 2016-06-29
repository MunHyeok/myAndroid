//
// Created by CHK on 2016-03-22.
//

#ifndef ANDROID_BLOB_H
#define ANDROID_BLOB_H
#include "cimUtils.h"
#include "BlobArea.h"
class CBlob
{
public:
    CvRect			m_rc;		// Blob의 사각 영역 (최초 영상 기준)
    int				m_color;	// Blob의 Color
    IplImage*		m_lab;		// 레이블 영상의 포인터
    bool			m_bValid;
    CvPoint2D32f	m_center;	// Blob의 중심 좌표 (원 영상 기준)

    CBlob(void) { m_bValid = false; }
    ~CBlob(void){}

    CvPoint2D32f	operator- ( CBlob &b)	{ return m_center - b.m_center; }


    //========================================================================
    // static members
    //========================================================================
    static CvMemStorage*	s_storage;
    static int				s_nBlobs;
    static CBlob*			s_blobs;
    static float			s_dist[];	// temporary float arrary
    static int		GetCount( ) { return s_nBlobs; }
    static CBlob*	GetBlobs( ) { return (s_nBlobs > 0) ? s_blobs : NULL; }

    static void		ResetBlobs();
    static int		FindBlobs( IplImage *src, IplImage *lab, int minArea=4 );
	static CBlobAreas*   FindBlobAreas(IplImage *src, int minArea = 4);
    // 1) K-means로부터 블롭의 유효 반지름을 추출
    static float EstimateBlobRadius ( float th=0.2f ) {
        if( s_nBlobs < 3 ) return -1.0f;

        for( int i=0 ; i<s_nBlobs ; i++ ) {
            CvRect& rc = s_blobs[i].m_rc;
            s_dist[i] = (float)(rc.width + rc.height);
        }
        float radius = cimKMeansMax( s_dist, s_nBlobs, 2 );
        for( int i=0 ; i<s_nBlobs ; i++ ) {
            CvRect& rc = s_blobs[i].m_rc;
            float rate = cvABS(rc.width + rc.height - radius) / radius;
            s_blobs[i].m_bValid = ( rate < th );
        }
        return radius / 2.f;
    }

    // 2) 남은 유효 블롭들만을 남김
    static int RemoveInvalidBlobs ( ) {
        for( int i=0 ; i<s_nBlobs ; i++ ) {
            if( s_blobs[i].m_bValid ) continue;
            s_blobs[i] = s_blobs[s_nBlobs-1];
            i--;
            s_nBlobs--;
        }
        for( int i=0 ; i<s_nBlobs ; i++ )
            s_blobs[i].m_center = cvCenter2D32f(s_blobs[i].m_rc);

        return s_nBlobs;
    }

    // 3) Blob들이 원형에 가까우며, 등간격의 2차원 배열로 이루어진 경우 Grid 간격을 찾음
    static CvPoint2D32f CalcBlobGridDist ( float radius, float minRate=0.5f, float maxRate=2.5f) {
        CvPoint2D32f gridDist;
        float mind = radius*minRate;
        float maxd = radius*maxRate;

        //가로 길이
        int cnt=0;
        for( int i=0 ; i<s_nBlobs ; i++ ) {
            CvPoint2D32f& p1 = s_blobs[i].m_center;
            for( int j=0 ; j<s_nBlobs ; j++ ) {
                if( i!=j ) {
                    CvPoint2D32f& p2 = s_blobs[j].m_center;
                    float dx = p2.x - p1.x;
                    float dy = p2.y - p1.y;
                    if( cvABS(dy)<mind && dx>mind && dx<maxd )
                        s_dist[cnt++] = dx;
                }
            }
        }
        gridDist.x = cimKMeansMax( s_dist, cnt, 2 );

        //세로 길이
        cnt=0;
        for( int i=0 ; i<s_nBlobs ; i++ ) {
            CvPoint2D32f& p1 = s_blobs[i].m_center;
            for( int j=0 ; j<s_nBlobs ; j++ ) {
                if( i!=j ) {
                    CvPoint2D32f& p2 = s_blobs[j].m_center;
                    float dx = p2.x - p1.x;
                    float dy = p2.y - p1.y;
                    if( cvABS(dx)<mind && dy>mind && dy<maxd )
                        s_dist[cnt++] = dy;
                }
            }
        }
        gridDist.y = cimKMeansMax( s_dist, cnt, 2 );
        return gridDist;
    }

    static CBlob* FindNearestBlob ( CvPoint2D32f& posi, float radius ) {
        int		minid = 0;
        float	mindist = (float)cvPointDist(posi, s_blobs[0].m_center );
        for( int i=1 ; i<s_nBlobs ; i++ ) {
            float	dist = (float)cvPointDist(posi, s_blobs[i].m_center );
            if( dist < mindist ) {
                minid = i;
                mindist = dist;
            }
        }
        return (mindist < radius) ? s_blobs+minid : NULL;
    }
};


#endif //ANDROID_BLOB_H
