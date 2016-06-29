
#include "Blob.h"

CvMemStorage*	CBlob::s_storage= NULL;
int				CBlob::s_nBlobs	= 0;
CBlob*			CBlob::s_blobs	= NULL;
float			CBlob::s_dist[4096];


// 하나의 Contour의 내부 영역을 그리기 위한 함수들.
/*
static int _color;
static inline void initColor() { _color = 255; }
static inline int  getColor()  { _color = (_color>1) ? _color-1 : 255; return _color; }
static inline int paintContour( IplImage *dst, CvSeq* contour )
{
	if( dst==NULL || contour==NULL ) return 0;
	else {
		int gray		= getColor();
		CvScalar color	= CV_RGB( gray, gray, gray );
		CvScalar hole	= CV_RGB(    0,    0,    0 );
		cvDrawContours( dst, contour, color, hole, 0, -1);
		return gray;
	}
}
*/
void CBlob::ResetBlobs()
{
    // 최초 한번만 실행
    if( s_storage == NULL )
        s_storage = cvCreateMemStorage();
    else {
        cvClearMemStorage( s_storage );
        if( s_blobs != NULL ) {
            delete [] s_blobs;
            s_blobs = NULL;
        }
    }
}

//===========================================================================
// 이진영상 src에서 모든 blob을 찾음
// 입력: src: 이진 영상 (8bit 1 channel)
//		lab: labeling 영상 (32bit int 1 channel)	
//		minArea: 최소 면적 (bounding box의 최소 크기)
// 출력: lab: 영상을 0으로 지우고 모든 blob을 고유의 색으로 coloring
//		찾아진 blob의 개수를 반환
//		blob 정보는 GetBlobs()함수를 통해 받아갈 수 있음
//

CBlobAreas* CBlob::FindBlobAreas(IplImage *src, int minArea){
	ResetBlobs();

	// 2. 영상을 준비함
	if (src == NULL) return NULL;
	//cvCopy(src, lab);
	//cvMorphologyEx( src, src, 0, 0, CV_MOP_OPEN, 1 );


	// 3. 영상내의 모든 External Boundary를 추출함
	int	    region_count = 0;
	CvSeq   *first_seq = NULL, *prev_seq = NULL, *seq = NULL, *outer = NULL;
	cvFindContours(src, s_storage, &first_seq, sizeof(CvContour),
		CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_NONE);

	// 4. 전체 Blob의 개수를 계산함 (사각 면적이 minArea인 Blob은 제외됨)
	s_nBlobs = 0;
	for (seq = first_seq; seq != NULL; seq = seq->h_next) {
		CvRect	theRc = ((CvContour*)seq)->rect;
		if (theRc.width * theRc.height >= minArea){
			s_nBlobs++;
		}
	}
	if (s_nBlobs == 0) return NULL;

	CBlobAreas* blobAreas = new CBlobAreas();

	blobAreas->data = new float[s_nBlobs];
	int	cnt = 0;
	for (seq = first_seq; seq != NULL; seq = seq->h_next) {
		 double data = cvContourArea(seq);
		 //data = sqrt(data / (2 * 1.414));
		 blobAreas->data[cnt] = data;
		//CvRect	theRc = ((CvContour*)seq)->rect;
		//if (theRc.width >= theRc.height) blobAreas->data[cnt] = theRc.height;
		//else if (theRc.width < theRc.height) blobAreas->data[cnt] = theRc.width;
		cnt++;
	}

	blobAreas->length = cnt;

	return blobAreas;

}

int CBlob::FindBlobs( IplImage *src, IplImage *lab, int minArea )
{
    // 1. memory storage를 설정함
    ResetBlobs();

    // 2. 영상을 준비함
    if( !cvIsGrayImage(src) || !cvSameSize(src, lab ) )		// !cvIsSameAll(src, lab )
        return 0;
    //cvCopy(src, lab);
    //cvMorphologyEx( src, src, 0, 0, CV_MOP_OPEN, 1 );


    // 3. 영상내의 모든 External Boundary를 추출함
    int	    region_count = 0;
    CvSeq   *first_seq = NULL, *prev_seq = NULL, *seq = NULL, *outer = NULL;
    cvFindContours( src, s_storage , &first_seq, sizeof(CvContour),
                    CV_RETR_EXTERNAL,
                    CV_CHAIN_APPROX_SIMPLE);

    // 4. 전체 Blob의 개수를 계산함 (사각 면적이 minArea인 Blob은 제외됨)
    s_nBlobs = 0;
    for( seq = first_seq; seq!=NULL ; seq = seq->h_next ) {
        CvRect	theRc = ((CvContour*)seq)->rect;
        if( theRc.width * theRc.height >= minArea) s_nBlobs++;
    }
    if( s_nBlobs == 0 ) return 0;

    // 5. Blob 정보 저장을 위한 메모리 할당 및 각 Blob 정보 저장
    s_blobs = new CBlob[s_nBlobs];
    cvSetZero( lab );
    int id = 0;
    for( seq = first_seq; seq!=NULL ; seq = seq->h_next ) {
        CvRect	rc = ((CvContour*)seq)->rect;
        if( rc.width * rc.height < minArea) continue;

        CBlob& blob = s_blobs[id++];		// 해당 blob 처리
        blob.m_lab	= lab;					// label 영상 저장
        blob.m_rc	= rc;					// rect 지정
        //blob.m_color= paintContour( lab, seq );	// lab 영상에 paint 하고, color를 m_color에 저장


        blob.m_color= id;					// lab 영상에 paint 하고, color를 m_color에 저장
        //CvScalar color	= CV_RGB( s_nBlobs-id, s_nBlobs-id, s_nBlobs-id );
        //CvScalar hole	= CV_RGB(    0,    0,    0 );
        CvScalar color	= cvRealScalar(s_nBlobs-id);
        CvScalar hole	= cvRealScalar(0);
        cvDrawContours( lab, seq, color, hole, 0, -1);

        //blob.m_bin	= src;						// 원 영상 저장
        //blob.m_area	= cvCalcArea  ( lab, &blob.m_rc, blob.m_color );
        //blob.m_center	= cvMassCenter( lab, &blob.m_rc, blob.m_color, &(blob.m_area) );
    }

    for( int i=0 ; i<s_nBlobs ; i++ ) {
        CBlob& blob = s_blobs[i];
        CvScalar color	= cvRealScalar(1000);
        //CvScalar color = CV_RGB(128,128,128);
        cvDrawRect( lab, blob.m_rc, color );
    }

    //	cvNormalize		 ( lab, lab, 0, 1., cv::NORM_MINMAX);
//    cimShowImage(lab, "Labeling image");

    return s_nBlobs;
}
