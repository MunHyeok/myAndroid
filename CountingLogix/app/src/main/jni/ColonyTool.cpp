#include "ColonyTool.h"

/*****************************************************************************
* Colony Count
*
*  1) 영상 이진화 문턱치 (Ostu) ==> m_th
*  2) 영상 이진화 ==> m_bin
*  3) 이진 영상 masking : m_bin & m_disc ==> m_bin
*  4) Dist Transformation : m_bin ==> m_dst
*  5) DistTr 영상 정규화: 0~1
*  6) DistTr 영상에서 Local Maximum 탐색 ==> m_lmax
*  7) 거리 조건을 이용해 Local Maximum Point 정리 ==> m_lmax
*  8) Cell 리스트에 추가
*
****************************************************************************/
int CColonyTool::CountColony(int offset, bool bInit)
{
    if( offset == 0 && bInit )
        m_th = cvRound(cvThreshold( m_gray.GetImage(), m_bin.GetImage(), 127, 255, CV_THRESH_OTSU));
    else {
        m_th += offset;
        cvThreshold( m_gray.GetImage(), m_bin.GetImage(), m_th, 255, CV_THRESH_BINARY);
    }

	cvMorphologyEx( m_bin.GetImage(), m_bin.GetImage(), 0, 0, CV_MOP_OPEN, 3 );

    cvMin(m_bin.GetImage(), m_dish.GetImage(), m_bin.GetImage());
	cvDistTransform	 ( m_bin.GetImage(), m_dst.GetImage() );
    cvNormalize		 ( m_dst.GetImage(), m_dst.GetImage(), 0, 1., cv::NORM_MINMAX);

    cvLocalMax32f	 ( m_dst.GetImage(), m_lmax.GetImage(), 2, 0.1f );
    cvTrimLocalMax32f( m_dst.GetImage(), m_lmax.GetImage(), 11 );

    int nCells = RegisterCells ( m_lmax.GetImage() );

        // 화면 출력 및 블롭 테스트: 임시 코드
    //    cimShowImage(m_dst.GetImage(), "Distance Tr");
    //    cimShowImage(m_bin.GetImage(), "Binary Image");
    //	FindBlobs( );

    //	CBlob::FindBlobs( m_bin.GetImage(), m_gray.GetImage() );
    CBlob::FindBlobs( m_bin.GetImage(), m_dst.GetImage() );

	return nCells;

}

int CColonyTool::RegisterCells( IplImage *max )
{
    m_nCells = 0;
    for( int y=0 ; y<cvHeight(max) ; y++ ) {
        unsigned char *c = cvGetPixelPtr(max, 0, y );
		 for( int x=0 ; x<cvWidth(max) ; x++ ) {
            if( c[x] == 255 )
                m_cell[m_nCells++].Set( x, y, 7.f );
            else if ( c[x] > 10 )
                m_cell[m_nCells++].Set( x, y, 3.f, false );
        }
    }
    return m_nCells;
}


/*
void CColonyTool::FindDish()
{
    int scale = cvRound( cvWidth(GetImage()) / 128.f );

    if (scale < 1) scale = 1;
    m_small.CopyReduce( GetImage(), scale );
    m_gray.CopyToGray( m_small.GetImage() );
    cvClearMemStorage(m_storage);

    double dp = 1;
    double min_dist=50;			// 100
    double cannyThreshold=60;	// 100
    double accThreshold=50;		// 50

    CvSeq* seqCircles = cvHoughCircles( m_gray.GetImage(), m_storage,
                CV_HOUGH_GRADIENT, dp, min_dist, cannyThreshold, accThreshold );

    m_nCircle = 0;
    for( int k = 0; k < seqCircles->total; k++ ) {
        float *circle = (float*)cvGetSeqElem(seqCircles, k);

        m_radius[m_nCircle] = circle[2] * scale;
        m_center[m_nCircle++] = cvPoint2D32f( circle[0] * scale, circle[1] * scale);
    }
}
*/
/*
void CColonyTool::AutoCount( )
{
	IplImage *src = m_dst.GetImage();
	IplImage *max = m_lmax.GetImage();

	cvLocalMax32f( src, max, 2, 0.1f );
	cvTrimLocalMax32f( src, max, 11 );

	m_nCircle = 0;

	for( int y=0 ; y<cvHeight(max) ; y++ ) {
		unsigned char *c = cvGetPixelPtr(max, 0, y );
		for( int x=0 ; x<cvWidth(max) ; x++ ) {
			if( c[x] > 0 ) {
				m_radius[m_nCircle] = 7;
				m_center[m_nCircle++] = cvPoint2D32f( x, y );
			}
		}
	}
	cimShowImage(max, "DstUChar");

	FindBlobs( );
}
*/


/*
	m_gray.CopyToGray( GetImage() );
	m_bin.Clone( &m_gray );

	cimShowImage(m_gray.GetImage(), "Gray");

	double th = cvThreshold( m_gray.GetImage(), m_bin.GetImage(), 127, 255, CV_THRESH_OTSU);
	cimShowImage(m_bin.GetImage(), "Ostu-127");

	cvThreshold( m_gray.GetImage(), m_bin.GetImage(), th, 255, CV_THRESH_BINARY);
	cimShowImage(m_bin.GetImage(), "Ostu-th");

	cvThreshold( m_gray.GetImage(), m_bin.GetImage(), th+30, 255, CV_THRESH_BINARY);
	cimShowImage(m_bin.GetImage(), "Ostu-th+50");

	cvThreshold( m_gray.GetImage(), m_bin.GetImage(), th, 255, CV_THRESH_BINARY | CV_ADAPTIVE_THRESH_MEAN_C);
	cimShowImage(m_bin.GetImage(), "AdaptiveMean");

	cvThreshold( m_gray.GetImage(), m_bin.GetImage(), th, 255, CV_THRESH_BINARY_INV | CV_ADAPTIVE_THRESH_GAUSSIAN_C);
	cimShowImage(m_bin.GetImage(), "AdaptiveGAUSSIAN");
*/


static int s_curr_gray = 255;
static int s_gray_diff = -1;
void cmSetPaintColor( int gray, int diff )
{
    s_curr_gray = gray;
    s_gray_diff = diff;
}

// 하나의 Contour의 내부 영역을 그린다.
void cmPaintContour( IplImage *dst, CvSeq* contour )
{
    if( dst==NULL || contour==NULL ) return;

    CvScalar color = CV_RGB( 0, 0, s_curr_gray );

    // maxlevel ==> 0 :  only contour is drawn.
    cvDrawContours( dst, contour, color, color, 0, -1);

    // contour의 color필드를 setting함
    ((CvContour*)contour)->color = s_curr_gray;

    // 다음 color를 setting 함
    s_curr_gray += s_gray_diff;
    if (s_curr_gray <= 0) s_curr_gray += 255;
}

// 모든 Contour의 내부 영역을 그린다.
void cmPaintContours( IplImage *dst, CvSeq* contour )
{
    if( dst==NULL || contour==NULL ) return;

    cmSetPaintColor( 255, -1 );
    cvZero(dst);

    for( ; contour != 0; contour = contour->h_next ) {
        cmPaintContour( dst, contour );
    }
}


void CColonyTool::FindBlobs( )
{
    IplImage *src = m_bin.GetImage();
    IplImage *lab = m_lmax.GetImage();

    cvCopy(src, lab);

    cvMorphologyEx( lab, lab, 0, 0, CV_MOP_OPEN, 1 );

    int	    region_count = 0;
    CvSeq   *first_seq = NULL, *prev_seq = NULL, *seq = NULL, *outer = NULL;
    cvClearMemStorage( m_storage );
    cvFindContours( lab, m_storage , &first_seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    cvCopy(src, lab);

    for( seq = first_seq; seq!=NULL ; seq = seq->h_next ) {
        CvRect	theRc = ((CvContour*)seq)->rect;
        bool	bDelete = false;

        // 조건 1: MBR의 크기가 작은 Contour는 제거
        if( theRc.width * theRc.height < 24)
            bDelete = true;

        // 위의 조건들에 의해 제거될 Contour로 판단되면 ==> Seq에서 제거... 몇 가지 예외처리
        if( bDelete ) {
            prev_seq = seq->h_prev;
            if( prev_seq ) {
                prev_seq->h_next = seq->h_next;
                if( seq->h_next ) seq->h_next->h_prev = prev_seq;
            }
            else {
                first_seq = seq->h_next;
                if( seq->h_next ) seq->h_next->h_prev = NULL;
            }
        }
        else {
            region_count++;
        }
    }
    cmPaintContours( lab, first_seq );
//    cimShowImage(lab, "Labeling image");
}


#ifdef XXX
/*****************************************************************************
* 배경 모델결과 만들어진 FG 영상에서 잡음 영향을 없앰
*
*  - Morphological Operation의 Opening/closing을 한번씩 처리
*  - 모든 영역에 대한 Contour를 추출함
*  - 영역의 크기가 일정 이하(params.minArea)인 Contour는 제거함
*  - Hole에 의한 Contour는 제거 (아직 구현 안됨)
*  - 추가적인 잡음 제거 방법?
*  -
*  - 최종적으로 FG 영상을 다시 만듦
****************************************************************************/
void cmBGModelNoiseRemoval( CmBGModel *bg_model )
{
	int	    region_count = 0;
	CvSeq   *first_seq = NULL, *prev_seq = NULL, *seq = NULL, *outer = NULL;

	cvMorphologyEx( bg_model->foreground, bg_model->foreground, 0, 0, CV_MOP_CLOSE, 1 );
	cvMorphologyEx( fg, fg, 0, 0, CV_MOP_OPEN, 1 );

	cvClearMemStorage( bg_model->storage );
	cvFindContours( fg, fg, &first_seq, sizeof(CvContour), CV_RETR_LIST );

	for( seq = first_seq; seq; seq = seq->h_next ) {
		CvRect	theRc = ((CvContour*)seq)->rect;
		bool	bDelete = false;

		// 조건 1: MBR의 크기가 작은 Contour는 제거
		if( theRc.width * theRc.height < bg_model->params.minArea )
			bDelete = true;

		// 조건 2: 다른 MBR에 포함된 Contour는 제거
		else {
			for( outer = first_seq ; outer ; outer = outer->h_next ) {
				if( outer == seq ) continue;	// 같은 contour 인 경우 ==> skip

				CvRect	rc = ((CvContour*)outer)->rect;

				if(cvIsInside( rc, cvPoint(theRc.x, theRc.y) )
					&& cvIsInside( rc, cvPoint(theRc.x + theRc.width, theRc.y + theRc.height) ) ) {
						bDelete = true;
						break;
				}
			}
		}

		// 위의 조건들에 의해 제거될 Contour로 판단되면 ==> Seq에서 제거... 몇 가지 예외처리
		if( bDelete ) {
			prev_seq = seq->h_prev;
			if( prev_seq ) {
				prev_seq->h_next = seq->h_next;
				if( seq->h_next ) seq->h_next->h_prev = prev_seq;
			}
			else {
				first_seq = seq->h_next;
				if( seq->h_next ) seq->h_next->h_prev = NULL;
			}
		}
		else {
			region_count++;
		}
	}
	bg_model->foreground_regions = first_seq;

	cmBGModelConvexHull( bg_model );
}

void cmBGModelConvexHull( CmBGModel *bg_model )
{
	CvSeq   *first_seq, *seq;
	CvSeq   *first_cvx, *cseq, *convex;

	first_seq = bg_model->foreground_regions;
	if( first_seq == NULL ) return;

	// list의 head
	first_cvx = cvConvexHull2 ( first_seq, bg_model->storage, CV_CLOCKWISE, 1 );
	/*
	first_cvx = cvApproxPoly ( first_seq, 
	sizeof(CvContour), 
	bg_model->storage, 
	CV_POLY_APPROX_DP, 
	1, 
	0 );
	*/
	cseq = first_cvx;
	cseq ->h_prev = NULL;
	cseq ->h_next = NULL;

	for( seq = first_seq->h_next ; seq ; seq = seq->h_next ) {
		convex = cvConvexHull2 ( seq, bg_model->storage, CV_CLOCKWISE, 1  );

		cseq->h_next = convex;
		convex->h_prev = cseq;
		convex->h_next = NULL;
		cseq = convex;
	}
	bg_model->foreground_regions = first_cvx;
}



static int s_curr_gray = 255;
static int s_gray_diff = -1;
void cmSetPaintColor( int gray, int diff )
{
	s_curr_gray = gray;
	s_gray_diff = diff;
}


/*****************************************************************************
* 배경 모델결과 만들어진 추출된 모든 등고선 영역에 대한 Minimum Bounding Rect.를 그림
*
*  - 배경제거가 축소된 영상에서 처리된 경우 --> 확대해서 그림
****************************************************************************/
void cmDrawMBR( IplImage *src, CmBGModel *bg_model )
{
	if( src==NULL || bg_model==NULL ) return;

	CvSeq   *first_seq = bg_model->foreground_regions;
	CvSeq   *prev_seq = NULL, *seq = NULL;

	for( seq = first_seq; seq; seq = seq->h_next ) {
		CvRect rc = ((CvContour*)seq)->rect;
		rc.x *= 2;
		rc.y *= 2;
		rc.width *= 2;
		rc.height *= 2;
		cvRectangle( src, 
			cvPoint( rc.x, rc.y ), 
			cvPoint( rc.x+rc.width, rc.y+rc.height ), 
			CV_RGB(255, 0, 0), 1 );
	}
}
#endif