//
// Created by CHK on 2016-03-22.
//

#ifndef ANDROID_CIMUTILS_H
#define ANDROID_CIMUTILS_H
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define	_USE_MATH_DEFINES
#include <math.h>

#define DEBUG_MODE
#ifdef DEBUG_MODE
#define Debug(x)    x
#else
#define Debug(x)
#endif

#define PrintMsg(msg,type)	MessageBox( NULL, type, msg, MB_OK )

#define CIP_Error( msg )	{ PrintMsg( "Error", msg ); exit( 0 ); }
#define CIP_Warning( msg )	PrintMsg( "Warning",msg )
#define CIP_Message( msg )	PrintMsg( "Message",msg )


#ifndef cvABS
#define cvABS(x)	(((x) > 0) ? (x) : -(x))
#endif

#ifndef cvMIN
#define cvMIN(x,y)	(((x) < (y)) ? (x) : (y))
#endif

#ifndef cvMAX
#define cvMAX(x,y)	(((x) > (y)) ? (x) : (y))
#endif


inline double Degree2Radian(double deg) { return M_PI * deg / 180; }
inline double Radian2Degree(double rad) { return 180 * rad / M_PI; }

//=============================================================================
// Geometry: Point
//	- arithmetic
//	- distance
//	- internal point
//
// Addition : p1 + p2
inline CvPoint	    cvPointAdd( CvPoint &p1, CvPoint &p2 )				{ return cvPoint     ( p1.x + p2.x, p1.y + p2.y ); }
inline CvPoint2D32f cvPointAdd( CvPoint2D32f &p1, CvPoint2D32f &p2 )	{ return cvPoint2D32f( p1.x + p2.x, p1.y + p2.y ); }
inline CvPoint2D64f cvPointAdd( CvPoint2D64f &p1, CvPoint2D64f &p2 )	{ return cvPoint2D64f( p1.x + p2.x, p1.y + p2.y ); }

// Subtraction : p1 - p2
inline CvPoint	    cvPointSub( CvPoint &pt1, CvPoint &pt2 )			{ return cvPoint     ( pt1.x - pt2.x, pt1.y - pt2.y ); }
inline CvPoint2D32f cvPointSub( CvPoint2D32f &pt1, CvPoint2D32f &pt2 )	{ return cvPoint2D32f( pt1.x - pt2.x, pt1.y - pt2.y ); }
inline CvPoint2D64f cvPointSub( CvPoint2D64f &pt1, CvPoint2D64f &pt2 )	{ return cvPoint2D64f( pt1.x - pt2.x, pt1.y - pt2.y ); }

// Vector magntude
inline double	    cvPointMag( CvPoint pt )							{ return sqrt( (double)(pt.x * pt.x + pt.y * pt.y) );	  }
inline double	    cvPointMag( CvPoint2D32f pt )						{ return sqrt( (double)(pt.x * pt.x + pt.y * pt.y) );	  }
inline double	    cvPointMag( CvPoint2D64f pt )						{ return sqrt( (double)(pt.x * pt.x + pt.y * pt.y) );	  }

// dist( p1, p2 )
inline double	    cvPointDist( CvPoint &pt1, CvPoint &pt2 )			{return cvPointMag( cvPointSub( pt2, pt1 ) );	  }
inline double	    cvPointDist( CvPoint2D32f &pt1, CvPoint2D32f &pt2 )	{return cvPointMag( cvPointSub( pt2, pt1 ) );	  }
inline double	    cvPointDist( CvPoint2D64f &pt1, CvPoint2D64f &pt2 )	{return cvPointMag( cvPointSub( pt2, pt1 ) );	  }

inline double	    cvInnerProduct( CvPoint &p1, CvPoint &p2 )			{ return (double)(p1.x*p2.x + p1.y*p2.y); }
inline double	    cvInnerProduct( CvPoint2D32f &p1, CvPoint2D32f &p2 ){ return (double)(p1.x*p2.x + p1.y*p2.y); }
inline double	    cvInnerProduct( CvPoint2D64f &p1, CvPoint2D64f &p2 ){ return (double)(p1.x*p2.x + p1.y*p2.y); }

inline CvPoint	    cvPointMult( CvPoint &p,		int    t )			{ return cvPoint     ( p.x*t, p.y*t); }
inline CvPoint2D32f cvPointMult( CvPoint2D32f &p,	float  t )			{ return cvPoint2D32f( p.x*t, p.y*t); }
inline CvPoint2D64f cvPointMult( CvPoint2D64f &p,	double t )			{ return cvPoint2D64f( p.x*t, p.y*t); }

// internal point (default:mid point)
inline CvPoint	    cvPointMid( CvPoint &p1, CvPoint &p2, double t=0.5 )			{ return cvPoint( cvRound(p1.x+(p2.x-p1.x)*t), cvRound(p1.y+(p2.y-p1.y)*t) ); }
inline CvPoint2D32f cvPointMid( CvPoint2D32f &p1, CvPoint2D32f &p2, float  t=0.5)	{ return cvPoint2D32f( p1.x+(p2.x-p1.x)*t, p1.y+(p2.y-p1.y)*t ); }
inline CvPoint2D64f cvPointMid( CvPoint2D64f &p1, CvPoint2D64f &p2, double t=0.5)	{ return cvPoint2D64f( p1.x+(p2.x-p1.x)*t, p1.y+(p2.y-p1.y)*t ); }


// float/double value ==> CvPoint 생성
inline CvPoint	cvPoint( float  x, float  y )				{ return cvPoint(cvRound(x), cvRound(y)); }
inline CvPoint	cvPoint( double x, double y )				{ return cvPoint(cvRound(x), cvRound(y)); }
inline CvPoint	cvPoint( CvPoint2D64f &p	)				{ return cvPoint(cvRound(p.x), cvRound(p.y)); }
inline CvPoint	cvPoint( CvPoint2D32f &p	)				{ return cvPoint(cvRound(p.x), cvRound(p.y)); }

// Maximum/Minimum point
inline void	cvSetMax( CvPoint& max,      CvPoint      pt ) { if(max.x < pt.x) max.x=pt.x; if(max.y < pt.y) max.y=pt.y; }
inline void	cvSetMax( CvPoint2D32f& max, CvPoint2D32f pt ) { if(max.x < pt.x) max.x=pt.x; if(max.y < pt.y) max.y=pt.y; }
inline void	cvSetMax( CvPoint2D64f& max, CvPoint2D64f pt ) { if(max.x < pt.x) max.x=pt.x; if(max.y < pt.y) max.y=pt.y; }
inline void	cvSetMin( CvPoint& min,      CvPoint      pt ) { if(min.x > pt.x) min.x=pt.x; if(min.y > pt.y) min.y=pt.y; }
inline void	cvSetMin( CvPoint2D32f& min, CvPoint2D32f pt ) { if(min.x > pt.x) min.x=pt.x; if(min.y > pt.y) min.y=pt.y; }
inline void	cvSetMin( CvPoint2D64f& min, CvPoint2D64f pt ) { if(min.x > pt.x) min.x=pt.x; if(min.y > pt.y) min.y=pt.y; }

inline CvPoint		cvOffset( CvPoint p1, CvPoint p2 )	{ return cvPoint(p1.x + p2.x, p1.y + p2.y); }
inline CvRect		cvOffset( CvRect  &rc, CvPoint pt )	{ return cvRect(rc.x+pt.x, rc.y+pt.y, rc.width, rc.height); }
inline CvRect		cvShrink( CvRect  &rc, int sc )			{ return cvRect(rc.x/sc, rc.y/sc, rc.width/sc, rc.height/sc); }
inline CvPoint2D32f	cvCenter2D32f( CvRect &rc)				{ return cvPoint2D32f( rc.x + rc.width*0.5, rc.y + rc.height*0.5 ); }
inline CvPoint2D64f	cvCenter( CvRect &rc)					{ return cvPoint2D64f( rc.x + rc.width*0.5, rc.y + rc.height*0.5 ); }
inline CvPoint2D64f	cvGetPt1( CvRect &rc)					{ return cvPoint2D64f( rc.x, rc.y ); }
inline CvPoint2D64f	cvGetPt2( CvRect &rc)					{ return cvPoint2D64f( rc.x+rc.width, rc.y+rc.height ); }
inline void cvGetCornerPoints( CvRect &rc, CvPoint2D64f pt[ ] ) {
    pt[0] = cvPoint2D64f( rc.x         , rc.y           );
    pt[1] = cvPoint2D64f( rc.x+rc.width, rc.y           );
    pt[2] = cvPoint2D64f( rc.x+rc.width, rc.y+rc.height );
    pt[3] = cvPoint2D64f( rc.x         , rc.y+rc.height );
}

// image boundary trimming (point)
inline void cvTrimBoundary( CvPoint &pt, CvSize size ) {
    if( pt.x < 0 ) { pt.x = 0; }
    if( pt.y < 0 ) { pt.y = 0; }
    if( pt.x >= size.width ) { pt.x = size.width - 1; }
    if( pt.y >= size.height) { pt.y = size.height- 1; }
}
// image boundary trimming (rectangle)
inline void cvTrimBoundary( CvRect &rc, CvSize size ) {
    if( rc.x < 0 ) { rc.width += rc.x; rc.x = 0; }
    if( rc.y < 0 ) { rc.height+= rc.y; rc.y = 0; }
    if( rc.x+rc.width > size.width ) { rc.width = size.width - rc.x; }
    if( rc.y+rc.height> size.height) { rc.height= size.height- rc.y; }
}


//=============================================================================
// Geometry: Rect
//
// Point + w ==> Rect
inline CvRect	cvPoint2Rect( CvPoint &pt, int    hw )	{ return cvRect( pt.x-hw, pt.y-hw, hw*2, hw*2 );  }
inline CvRect	cvPoint2Rect( CvPoint &pt, double hw )	{ return cvPoint2Rect( pt, cvRound(hw));  }

// Rect ==> Center Point
inline CvPoint2D32f cvRectCenter2D32f( CvRect &rc )	{ return cvPoint2D32f( rc.x + rc.width*0.5, rc.y + rc.height*0.5 ); }
inline CvPoint	    cvRectCenter    ( CvRect &rc )	{ return cvPoint( cvRound(rc.x+rc.width*0.5), cvRound(rc.y+rc.height*0.5)); }

// float ==> CvRect
inline CvRect	cvRect( float x1, float y1, float x2, float y2) {
    return cvRect( (int)(x1+0.5), (int)(y1+0.5), (int)(x2+0.5), (int)(y2+0.5));
}
// double ==> CvRect
inline CvRect	cvRect( double x1, double y1, double x2, double y2) {
    return cvRect( (int)(x1+0.5), (int)(y1+0.5), (int)(x2+0.5), (int)(y2+0.5));
}

inline CvRect	cvRect( CvPoint p1, CvPoint p2 ) {
    return cvRect( cvMIN(p1.x, p2.x), cvMIN(p1.y, p2.y), cvABS(p2.x-p1.x), cvABS(p2.y-p1.y));
}

inline int	cvArea( CvRect &rc ) { return rc.width * rc.height; }


// Point Inside Test
inline bool	cvIsInside( CvRect rc, CvPoint pt ) {
    return( pt.x>=rc.x && pt.x < rc.x+rc.width
            &&	pt.y>=rc.y && pt.y < rc.y+rc.height
    );
}
inline bool	cvIsInside( CvRect rc, CvPoint2D32f &pt ) { return cvIsInside(rc, cvPoint( pt.x, pt.y) ); }

// CvSize 크기 비교
inline bool	cvSameSize( CvSize s1, CvSize s2 ) { return (s1.width == s2.width && s1.height == s2.height); }
inline bool	cvSameSize( CvRect r1, CvRect r2 ) { return (r1.width == r2.width && r1.height == r2.height); }
inline CvRect	cvEnlarge( CvRect rc, int dist ) {
    return cvRect( rc.x-dist, rc.y-dist,
                   rc.width +dist*2,
                   rc.height+dist*2);
}


//=============================================================================
// Drawing functions
//	- Rect
//	- Text
// Draw Rect
inline void	cvDrawRect( CvArr* img, CvRect &rc, CvScalar color, int thickness=1, int line_type=8, int shift=0 ) {
    cvRectangle( img,	cvPoint( rc.x, rc.y ),
                 cvPoint( rc.x+rc.width, rc.y+rc.height ),
                 color, thickness, line_type, shift );
}
extern void cvDrawText( IplImage *src, char const  *msg, CvPoint pt,
                        int font=5,
                        CvScalar color = CV_RGB(255,255,0),
                        float scale=1.0f,
                        int thick = 1  );


//=============================================================================
// IplImage 자료구조 관련/
//	- validity, size, type
//	- pixel position
//	- ROI

// image type check : need improvement...
inline bool cvIsRGBImage ( IplImage *img )				{ return (img!=NULL && img->nChannels==3); }
inline bool cvIsGrayImage( IplImage *img )				{ return (img!=NULL && img->nChannels==1); }
inline bool cvSameDepth( IplImage* i1, IplImage* i2 )	{ return (i1 && i2 && i1->depth==i2->depth );			}
inline bool cvSameChannel(IplImage* i1,IplImage* i2 )	{ return (i1 && i2 && i1->nChannels==i2->nChannels ); }
inline bool cvSameSize ( CvSize& a, CvSize& b )			{ return (a.width==b.width && a.height==b.height); }
inline bool cvSameSize ( IplImage* i1, IplImage* i2 )	{ return (i1 && i2 && cvSameSize(cvGetSize(i1), cvGetSize(i2))); }
inline bool cvIsSameAll( IplImage* i1, IplImage* i2 )	{ return (cvSameSize(i1, i2) && cvSameDepth(i1, i2) && cvSameChannel(i1, i2)); }

inline void cvCopySmart ( IplImage* s, IplImage** d ) {
    if( s!=NULL && d!=NULL ) {
        if( *d!=NULL && !cvIsSameAll(s, *d) ) cvReleaseImage( d );
        if( *d==NULL ) *d = cvCloneImage( s );
        else cvCopy( s, *d );
    }
}
inline void cvCreateSmart ( CvSize sz, int depth, int nch, IplImage** d ) {
    if( d==NULL ) return;
    if( *d!=NULL ) {
//        if( !cvSameSize( sz, cvGetSize(*d) )
//            ||  depth != (*d)->depth
//            ||  nch	  != (*d)->nChannels ) cvReleaseImage( d );	// reset prev image
//        else return;	// no need to re-create
        return;
    }
    if( *d==NULL )
        *d = cvCreateImage( sz, depth, nch );
}

// get pixel value / pointer
inline unsigned char cvGetUcharPixel( IplImage *img, int x, int y ) { return *((unsigned char*)img->imageData + y*img->widthStep + x); }
inline unsigned char cvGetUcharPixel( IplImage *img, CvPoint p )    { return *((unsigned char*)img->imageData + p.y*img->widthStep + p.x); }

inline unsigned char* cvGetPixelPtr ( IplImage *img, int x, int y ){
    return ( img->roi == NULL )
           ? ((unsigned char*)img->imageData + y*img->widthStep + x*img->nChannels)
           : ((unsigned char*)img->imageData + (y+img->roi->yOffset)*img->widthStep + (x+img->roi->xOffset)*img->nChannels);
}
inline unsigned char* cvGetPixelPtr ( IplImage *img, CvPoint p )	{return (cvGetPixelPtr ( img, p.x, p.y )); }
inline float* cvGetFloat ( IplImage *img, int x, int y ){
    return (float*)(
            ( img->roi == NULL )
            ? ((unsigned char*)img->imageData + y*img->widthStep + x*img->nChannels * img->depth/8)
            : ((unsigned char*)img->imageData + (y+img->roi->yOffset)*img->widthStep + (x+img->roi->xOffset)*img->nChannels * img->depth/8)
    );
}

//inline unsigned char& cvGetPixel( IplImage *img, int x, int y )	    { return *((unsigned char*)img->imageData + y*img->widthStep + x); }
//inline unsigned char& cvGetPixel( IplImage *img, CvPoint p )	    { return *((unsigned char*)img->imageData + p.y*img->widthStep + p.x); }


inline CvRect cimClipRect( CvRect rc, CvRect rcOrg )
{
    int minx = rc.x;
    int maxx = rc.x + rc.width;
    int miny = rc.y;
    int maxy = rc.y + rc.height;

    if( minx < rcOrg.x ) minx = rcOrg.x;
    if( miny < rcOrg.y ) miny = rcOrg.y;
    if( maxx > rcOrg.x+rcOrg.width ) maxx = rcOrg.x+rcOrg.width ;
    if( maxy > rcOrg.y+rcOrg.height) maxy = rcOrg.y+rcOrg.height;

    return cvRect( minx, miny, (maxx-minx), (maxy-miny) );
}
inline unsigned char& cimGetPixel( IplImage *img, int x, int y, CvRect *rc = NULL )
{
    return (rc == NULL)
           ? *((unsigned char*)img->imageData + y*img->widthStep + x)
           : *((unsigned char*)img->imageData + (y+rc->y) *img->widthStep + x + rc->x);
}
inline unsigned char& cvGetPixel( IplImage *img, int x, int y )
{
    return *((unsigned char*)img->imageData + y*img->widthStep + x);
}
inline unsigned char& cvGetPixel( IplImage *img, CvPoint p )
{
    return *((unsigned char*)img->imageData + p.y*img->widthStep + p.x);
}
inline void cvShowImageRect( char *name, IplImage *img, CvRect *rc )
{
    if( name==NULL || img==NULL || rc==NULL ) return;

    cvSetImageROI( img,  *rc );
    cvNamedWindow( name );
    cvShowImage( name, img );
    cvResetImageROI( img );
}
inline void cvRectTlBr( CvPoint& p, CvPoint& q ) {
    if( p.x > q.x ) { int tmp = p.x; p.x = q.x; q.x = tmp; }
    if( p.y > q.y ) { int tmp = p.y; p.y = q.y; q.y = tmp; }
}


//=======================================================================
// ROI 관련 처리는 매우 신중하게 해야 함.
// 정확한 개념을 이해해야 함. 조심조심.
inline IplImage cvGetWindow( IplImage *src, CvRect rc ) {
    IplImage win = *src;
    win.roi = NULL;
    if( src->roi != NULL ) {
        rc = cvOffset(rc, cvPoint(src->roi->xOffset, src->roi->yOffset) );
    }
    cvSetImageROI( &win,  rc );
    return win;
}
//=======================================================================
inline int cvWidth ( IplImage *src ) { return (src->roi == NULL) ? src->width : src->roi->width;  }
inline int cvHeight( IplImage *src ) { return (src->roi == NULL) ? src->height: src->roi->height; }
inline CvRect cvRect( IplImage *src ) {
    return (src->roi == NULL)
           ? cvRect(0, 0, src->width, src->height)
           : cvRect(src->roi->xOffset, src->roi->yOffset, src->roi->width, src->roi->height);
}
inline CvRect cvGetRect( IplImage *src ) {
    return (src==NULL) ? cvRect(0,0,0,0) : cvRect(src);
}
inline CvSize cvSize( IplImage *src ) {
    return (src->roi == NULL)
           ? cvSize(src->width, src->height)
           : cvSize(src->roi->width, src->roi->height);
}


extern IplImage		cvGetTmp( int w, int h, int x=0, int y=0 );
extern const char*	cvGetCurrTime();


// ROI
inline void cvCopyWindow ( IplImage* src, CvRect srcRc,  IplImage* dst, CvRect dstRc) {
    if( src!=NULL && dst!=NULL ) {
        cvSetImageROI( src, srcRc );
        cvSetImageROI( dst, dstRc );
        cvResize( src, dst );
        cvResetImageROI( dst );
        cvResetImageROI( src );
    }
}
inline void cvCopyWindowFramed ( IplImage* src, IplImage* dst, CvRect rc, int thickness ) {
    if( src!=NULL && dst!=NULL ) {
        CvRect rcEnc = cvEnlarge( rc, thickness );
        rcEnc = cimClipRect( rcEnc, cvRect( 1, 1, src->width-2, src->height-2 ) );
        cvSetImageROI( dst, rcEnc );
        cvZero( dst );
        cvCopyWindow( src, rc, dst, rc );
    }
}


inline void cvRotateCenter( IplImage* src, IplImage* dst, double angle, int interpolation=CV_INTER_LINEAR ) {
    if( src!=NULL || dst!=NULL ) {
        CvPoint2D32f center = cvPoint2D32f (src->width/2.0, src->height/2.0);
        CvMat		*rotMat = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix ( center, angle, 1, rotMat );
        cvWarpAffine( src, dst, rotMat,
                      interpolation + CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );
        cvReleaseMat( &rotMat );
    }
}

// 파일 형식: Rect이름 10 10 30 40 <enter>
inline void cvPrintRect( FILE *fp, CvRect rc, const char *name = "Rect" ) {
    if( fp != NULL ) {
        fprintf(fp, "%s\t%4d %4d %4d %4d\n", name, rc.x, rc.y, rc.width, rc.height );
    }
}
inline bool cvReadRect( char *line, CvRect &rc, const char *type="Rect" ) {
    if( line == NULL ) return false;
    char name[100];
    if( sscanf(line, "%s%d%d%d%d", name, &rc.x, &rc.y, &rc.width, &rc.height ) != 5 )
        return false;

    return strcmp(name, type)==0;
}



extern void cimShowImage( IplImage *img, const char *name=0, int scale=0, bool bHori=true );


//=============================================================================
// cimProj.cpp
//	- image projection/histogram 및 1차원 배열의 간단한 처리를 위한 함수들
//	- 1차원 배열은 간단하지만 효율성을 높이기 위해 float array로 처리
//	- 2015-04-23
//=============================================================================
// Subpixel operation을 통한 정밀 위치 계산 함수
inline float	cimSubPixelPosi( float *e, int i ) { return i - (e[i-1]-e[i+1]) / (4*e[i]-2*e[i-1]-2*e[i+1]); }

extern void		cimNormalizeArray( float *prj, int len, float maxVal );
extern void		cimCalcAvgSdv( float *prj, int len, float& mean, float& stdDev );
extern bool		cimIsLocalMin( float *prj, int len, int mini, int winsize );
extern bool		cimIsLocalMax( float *prj, int len, int maxi, int winsize );
extern float	cimLocalMaxDiff( float *prj, int len, int maxi, int winsize );

extern void		cimFindMinMax( float *hist, int sx, int ex, float* pmin, float* pmax );
inline void		cimFindMinMax( float *hist, int len,        float* pmin, float* pmax )	{ cimFindMinMax( hist, 0, len, pmin, pmax ); }
inline void		cimFindMinMax( float *hist, int sx, int ex, float &minv, float &maxv )	{ cimFindMinMax( hist, sx, ex,&minv,&maxv ); }
inline void		cimFindMinMax( float *hist, int len,        float &minv, float &maxv )	{ cimFindMinMax( hist, 0, len,&minv,&maxv ); }

extern void		cimFindMinMax( unsigned char *img, int sx, int ex, int &minv, int &maxv );

extern float	cimFindMaxPosi( float *prj, int si, int ei, bool bSubPixel );
extern float	cimFindMinPosi( float *prj, int si, int ei, bool bSubPixel );
inline float	cimFindMaxPosi( float *prj, int len, bool bSubPixel ) { return cimFindMaxPosi( prj, 0, len, bSubPixel ); }
inline float	cimFindMinPosi( float *prj, int len, bool bSubPixel ) { return cimFindMinPosi( prj, 0, len, bSubPixel ); }



// 현재 위치가 Falling/Rising edge인지를 판단하는 함수
inline bool		cimIsFalling( float *e, int i, float mag=3.f )	{ return( e[i] < -mag &&  e[i] <= e[i-1] &&  e[i] <  e[i+1] ); }
inline bool		cimIsRising ( float *e, int i, float mag=3.f )	{ return( e[i] >  mag &&  e[i] >= e[i-1] &&  e[i] >  e[i+1] ); }
inline bool		cimIsFalling2(float *e, int i, float mag=3.f )	{ return( cimIsFalling(e,i,mag) && e[i] <= e[i-2] &&  e[i] <  e[i+2] ); }
inline bool		cimIsRising2( float *e, int i, float mag=3.f )	{ return( cimIsRising (e,i,mag) && e[i] >= e[i-2] &&  e[i] >  e[i+2] ); }

extern float	cimNextRise( float *e, int i, int w, float mag );
extern float	cimNextFall( float *e, int i, int w, float mag );
extern float	cimNextPeak( float *e, int i, int w, float mag );


// 프로젝션 프로파일을 구하는 함수들
extern float*	cimProjH( IplImage *img, float *prj = NULL );
extern float*	cimProjV( IplImage *img, float *prj = NULL );
extern float*	cimEdge ( float *prj, int len, float *edge = NULL );
extern float*	cimEdge2( float *prj, int len, float *edge = NULL );
extern int		cimSumHLine (IplImage *src, int x, int y, int width );
extern int		cimSumVLine (IplImage *src, int x, int y, int height);
extern float*	cimProjV( IplImage *img, float *prj, float *h, float th );

extern float*	cimFindRisingPosi ( float *e, int len, int* pcnt, float mag=3.0f );
extern float*	cimFindFallingPosi( float *e, int len, int* pcnt, float mag=3.0f );


// 프로젝션/히스토그램을 영상에 출력하는 함수들
extern void		cimDispNormV( float *prj, IplImage *img, bool bOverlap=true, bool toTop =true );
extern void		cimDispNormH( float *prj, IplImage *img, bool bOverlap=true, bool toLeft=true );
extern void		cimDispHistV( float *prj, IplImage *img, float min, float max, bool bOverlap=true, bool toTop=true );
extern void		cimDispHistH( float *prj, IplImage *img, float min, float max, bool bOverlap=true, bool toLeft=true );

inline void		cimDispNormV( float *prj, IplImage *img, CvRect rc, bool bOverlap=true, bool toTop =true ) {
    IplImage win = cvGetWindow( img, rc );
    cimDispNormV( prj, &win, bOverlap, toTop );
}
inline void		cimDispNormH( float *prj, IplImage *img, CvRect rc, bool bOverlap=true, bool toLeft =true ) {
    IplImage win = cvGetWindow( img, rc );
    cimDispNormH( prj, &win, bOverlap, toLeft );
}
inline void		cimDispHistV( float *prj, IplImage *img, CvRect rc, float min, float max, bool bOverlap=true, bool toTop=true )  {
    IplImage win = cvGetWindow( img, rc );
    cimDispHistV( prj, &win, min, max, bOverlap, toTop );
}

inline void		cimDispHistH( float *prj, IplImage *img, CvRect rc, float min, float max, bool bOverlap=true, bool toLeft=true )  {
    IplImage win = cvGetWindow( img, rc );
    cimDispHistH( prj, &win, min, max, bOverlap, toLeft );
}




extern float*	cimKMeans1D ( float *p, int len, int k );
extern float	cimKMeansMax( float *p, int len, int k );
extern float	cimKMeansDiff(float *p, int len, int k );


extern bool		cvCirclePPP( CvPoint2D32f& p1, CvPoint2D32f& p2, CvPoint2D32f& p3, CvPoint2D32f& center, float &radius );
extern bool		cvLocalMax32f( IplImage *src, IplImage *dst, int dist=2, float th=0.1f );
extern bool		cvTrimLocalMax32f( IplImage *src, IplImage *max, int dist );


extern float*	cimMovingAvg ( float *p, int len, int hw, float *q=NULL );


extern int		cvCalcArea  ( IplImage *lab, CvRect *rc, int gray );
extern CvPoint2D32f cvMassCenter( IplImage *lab, CvRect *rc, int gray, int *pArea=NULL );





//=============================================================================
// operator overloading
//
inline CvPoint2D32f  operator*	(CvPoint2D32f &p,	float t)		{ return cvPointMult(p, t); }
inline CvPoint2D32f  operator*	(float t, CvPoint2D32f &p)			{ return cvPointMult(p, t); }
inline CvPoint2D32f  operator/	(CvPoint2D32f &p,	float t)		{ return cvPointMult(p, 1.0f/t); }
inline CvPoint2D32f& operator/=	(CvPoint2D32f &p,	float t)		{ p.x/=t; p.y/=t; return p; }
inline CvPoint2D32f& operator+=	(CvPoint2D32f &p, CvPoint2D32f &q)	{ p.x+=q.x; p.y+=q.y; return p; }
inline CvPoint2D32f& operator-=	(CvPoint2D32f &p, CvPoint2D32f &q)	{ p.x-=q.x; p.y-=q.y; return p; }
inline CvPoint2D32f	 operator+	(CvPoint2D32f &p, CvPoint2D32f &q)	{ return cvPoint2D32f(p.x+q.x, p.y+q.y); }
inline CvPoint2D32f	 operator-	(CvPoint2D32f &p, CvPoint2D32f &q)	{ return cvPoint2D32f(p.x-q.x, p.y-q.y); }

#endif //ANDROID_CIMUTILS_H
