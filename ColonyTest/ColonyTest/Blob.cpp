#include "Blob.h"
#include "BlobArea.h"
CvMemStorage*	CBlob::s_storage = NULL;
int				CBlob::s_nBlobs = 0;
CBlob*			CBlob::s_blobs = NULL;
float			CBlob::s_dist[4096];


// �ϳ��� Contour�� ���� ������ �׸��� ���� �Լ���.
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
	// ���� �ѹ��� ����
	if (s_storage == NULL)
		s_storage = cvCreateMemStorage();
	else {
		cvClearMemStorage(s_storage);
		if (s_blobs != NULL) {
			delete[] s_blobs;
			s_blobs = NULL;
		}
	}
}

//===========================================================================
// �������� src���� ��� blob�� ã��
// �Է�: src: ���� ���� (8bit 1 channel)
//		lab: labeling ���� (32bit int 1 channel)	
//		minArea: �ּ� ���� (bounding box�� �ּ� ũ��)
// ���: lab: ������ 0���� ����� ��� blob�� ������ ������ coloring
//		ã���� blob�� ������ ��ȯ
//		blob ������ GetBlobs()�Լ��� ���� �޾ư� �� ����
//

CBlobAreas* CBlob::FindBlobAreas(IplImage *src, int minArea){
	ResetBlobs();

	// 2. ������ �غ���
	if (src == NULL) return NULL;
	//cvCopy(src, lab);
	//cvMorphologyEx( src, src, 0, 0, CV_MOP_OPEN, 1 );


	// 3. ������ ��� External Boundary�� ������
	int	    region_count = 0;
	CvSeq   *first_seq = NULL, *prev_seq = NULL, *seq = NULL, *outer = NULL;
	cvFindContours(src, s_storage, &first_seq, sizeof(CvContour),
		CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_NONE);

	// 4. ��ü Blob�� ������ ����� (�簢 ������ minArea�� Blob�� ���ܵ�)
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
int CBlob::FindBlobs(IplImage *src, IplImage *lab, int minArea)
{
	// 1. memory storage�� ������
	ResetBlobs();


	// 2. ������ �غ���
	if (!cvIsGrayImage(src) || !cvSameSize(src, lab))		// !cvIsSameAll(src, lab )
		return 0;
	//cvCopy(src, lab);
	//cvMorphologyEx( src, src, 0, 0, CV_MOP_OPEN, 1 );


	// 3. ������ ��� External Boundary�� ������
	int	    region_count = 0;
	CvSeq   *first_seq = NULL, *prev_seq = NULL, *seq = NULL, *outer = NULL;
	cvFindContours(src, s_storage, &first_seq, sizeof(CvContour),
		CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_NONE);

	// 4. ��ü Blob�� ������ ����� (�簢 ������ minArea�� Blob�� ���ܵ�)
	s_nBlobs = 0;
	for (seq = first_seq; seq != NULL; seq = seq->h_next) {
		CvRect	theRc = ((CvContour*)seq)->rect;
		if (theRc.width * theRc.height >= minArea) s_nBlobs++;
	}
	if (s_nBlobs == 0) return 0;

	// 5. Blob ���� ������ ���� �޸� �Ҵ� �� �� Blob ���� ����
	s_blobs = new CBlob[s_nBlobs];
	cvSetZero(lab);
	int id = 0;
	for (seq = first_seq; seq != NULL; seq = seq->h_next) {
		CvRect	rc = ((CvContour*)seq)->rect;
		if (rc.width * rc.height < minArea) continue;

		CBlob& blob = s_blobs[id++];		// �ش� blob ó��
		blob.m_lab = lab;					// label ���� ����
		blob.m_rc = rc;					// rect ����
		//blob.m_color= paintContour( lab, seq );	// lab ���� paint �ϰ�, color�� m_color�� ����


		blob.m_color = id;					// lab ���� paint �ϰ�, color�� m_color�� ����
		//CvScalar color	= CV_RGB( s_nBlobs-id, s_nBlobs-id, s_nBlobs-id );
		//CvScalar hole	= CV_RGB(    0,    0,    0 );
		CvScalar color = cvRealScalar(s_nBlobs - id);
		CvScalar hole = cvRealScalar(0);
		cvDrawContours(lab, seq, color, hole, 0, -1);

		//blob.m_bin	= src;						// �� ���� ����
		//blob.m_area	= cvCalcArea  ( lab, &blob.m_rc, blob.m_color );
		//blob.m_center	= cvMassCenter( lab, &blob.m_rc, blob.m_color, &(blob.m_area) );
	}

	for (int i = 0; i<s_nBlobs; i++) {
		CBlob& blob = s_blobs[i];
		CvScalar color = cvRealScalar(1000);
		//CvScalar color = CV_RGB(128,128,128);
		cvDrawRect(lab, blob.m_rc, color);
	}

	//	cvNormalize		 ( lab, lab, 0, 1., cv::NORM_MINMAX);
	//cimShowImage(lab, "Labeling image");
	cvShowImage("Labeling image", lab);
	return s_nBlobs;
}
