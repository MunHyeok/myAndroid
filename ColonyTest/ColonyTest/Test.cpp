
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
#include "ColonyCell.h"
#include "cimUtils.h"
#include <iostream>
#include <stdio.h>
#include "Matrix.h"
#include "Blob.h"
#include "RollingBall.h"
using namespace cv;


#define	MAX_NUM_CLUSTER	2048
#define DIST(x,y)	((x)>(y)) ? ((x)-(y)) : ((y)-(x))
static	float	_mean[MAX_NUM_CLUSTER];
static	int		_cnt[MAX_NUM_CLUSTER];

float* cimKMeans1D(float *p, int len, int k)
{
	if (p == NULL || len <= k) return NULL;

	float	posi[MAX_NUM_CLUSTER];
	bool	bChanged = true;
	float	mind, dst, d, mean;
	int		i, n, mink;

	// 최초 cluster 별 평균값 초기화 
	for (i = 0; i<k; i++)
		_mean[i] = p[i];


	while (bChanged) {
		for (i = 0; i<k; i++) {
			_cnt[i] = 0;
			posi[i] = 0.0f;
		}

		for (n = 0; n<len; n++) {
			d = p[n];
			mink = 0;
			mind = DIST(d, _mean[mink]);

			// 가장 가까운 평균을 찾음
			for (i = 1; i<k; i++) {
				dst = DIST(d, _mean[i]);
				if (mind > dst) {
					mind = dst;
					mink = i;
				}
			}

			// 새로운 평균 계산을 위해 해당 위치의 값 및 count 추가 
			posi[mink] += d;
			_cnt[mink]++;
		}

		// 평균값 변경 : 변경이 있으면 ==> 다시 반복
		bChanged = false;
		for (i = 0; i<k; i++) {
			mean = posi[i] / _cnt[i];
			if (_mean[i] > mean + 0.000001f || _mean[i] < mean - 0.000001f) {
				bChanged = true;
				_mean[i] = mean;
			}
		}
	}
	return _mean;
}
float cimKMeansMax(float *p, int len, int k)
{
	float	*mean = cimKMeans1D(p, len, k);

	if (mean != NULL) {
		//int max = 0;
		//for (int i = 1; i<k; i++)
		//if (_cnt[i] > _cnt[max]) max = i;

		//return mean[max];
		int min = 0;
		for (int i = 1; i < k; i++){
			if (mean[min] > mean[i]) min = i;
		}
		
		return mean[min];

	}
	else return 0.0f;
}

float cimKMeansDiff(float p[], int len, int k)
{
	static float	diff[4096];
	for (int i = 0; i<len - 1; i++)
		diff[i] = p[i + 1] - p[i];

	return cimKMeansMax(diff, len - 1, k);
}



CColonyCell* m_cell;

int RegisterCells(IplImage *max)
{
	int m_nCells = 0;
	m_cell = new CColonyCell[10000];
	for (int y = 0; y<cvHeight(max); y++) {
		unsigned char *c = cvGetPixelPtr(max, 0, y);
		for (int x = 0; x<cvWidth(max); x++) {
			if (m_nCells <= 9999){
				if (c[x] == 255)
					m_cell[m_nCells++].Set(x, y, 7.f);
				else if (c[x] > 10)
					m_cell[m_nCells++].Set(x, y, 3.f, false);
			}
			else
				break;
			
		}
	}
	return m_nCells;
}

void main(){

	IplImage* image = cvLoadImage("test21.png");
	IplImage* resizedImage = cvCreateImage(cvSize(image->width/2, image->height/2), IPL_DEPTH_8U, 3);
	IplImage* resizedImage2 = cvCreateImage(cvSize(image->width / 2, image->height / 2), IPL_DEPTH_8U, 3);

	cvNot(image, image);
	//IplImage* test = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	//cvCvtColor(image, test, CV_RGB2GRAY);
	//RollingBall::subtractBackground(test, test, 100);
	//cvShowImage("테스트", test);
	/*IplImage* resizedImage = cvCreateImage(cvSize(500, 500), IPL_DEPTH_8U, 3);
	IplImage* resizedImage2 = cvCreateImage(cvSize(500, 500), IPL_DEPTH_8U, 3);
*/
	IplImage* grayImage = NULL;
	IplImage* binImage = NULL;
	IplImage* dstImage = NULL;
	cvNot(image, image);
	cvResize(image, resizedImage, CV_INTER_CUBIC);
	cvResize(image, resizedImage2, CV_INTER_CUBIC);

	cvShowImage("ori", resizedImage);
	grayImage = cvCreateImage(cvGetSize(resizedImage), IPL_DEPTH_8U, 1);

	cvCvtColor(resizedImage, grayImage, CV_RGB2GRAY);
	binImage = cvCloneImage(grayImage);

	cvThreshold(binImage, binImage, 0, 255, CV_THRESH_OTSU);
	cvMorphologyEx(binImage, binImage, 0, 0, CV_MOP_OPEN, 3);
	dstImage = cvCreateImage(cvSize(grayImage->width, grayImage->height), 32, 1);


	cvShowImage("binary", binImage);


	CBlobAreas* blobAreas = CBlob::FindBlobAreas(binImage);
	double data = (double)cimKMeansMax(blobAreas->data, blobAreas->length, 3);
	int radius = (int)sqrt(data / (3.141592));
	printf_s("%d\n", radius);
	
	
	CBlob::FindBlobs(binImage, dstImage);

	cvDistTransform(binImage, dstImage);
	cvNormalize(dstImage, dstImage, 0, 1., cv::NORM_MINMAX);

	cvShowImage("distance1", dstImage);


	if (radius < 4){
		cvLocalMax32f(dstImage, dstImage, 2, 0.1f);
		cvTrimLocalMax32f(dstImage, grayImage, 11);
	}
	else{
		cvSmooth(dstImage, dstImage, CV_GAUSSIAN, radius * 2 - 1);
		cvLocalMax32f(dstImage, grayImage, (int)radius / 4, 0.1f);
	}

	//cvSmooth(dstImage, dstImage, CV_GAUSSIAN, radius * 2 - 1); 
	////cvSmooth(dstImage, dstImage, CV_GAUSSIAN, radius);
	//
	//cvLocalMax32f(dstImage, grayImage, (int)radius/4, 0.1f);
	
	printf("%d\n", (int)radius / 4);
	//cvLocalMax32f(dstImage, grayImage, 2, 0.1f);
	
	//cvTrimLocalMax32f(dstImage, grayImage, 11);

	CvRect a;
	a.x = 100;
	a.y = 100;
	a.width = radius * 2;
	a.height = radius * 2;
	CvScalar color = cvRealScalar(2000);
	cvDrawRect(resizedImage, a, color);

	int nCells = RegisterCells(grayImage);

	for (int i = 0; i < nCells; i++){
		m_cell[i].Draw(resizedImage);
	}

	printf("개수 : %d \n", nCells); 
	
	cvNot(resizedImage, resizedImage);
	cvNot(resizedImage2, resizedImage2);

	cvShowImage("resized", resizedImage);
	cvShowImage("distance", dstImage);
	cvShowImage("origin", resizedImage2);
	cvWaitKey(0);
	cvReleaseImage(&resizedImage);
	cvReleaseImage(&grayImage);
	cvReleaseImage(&binImage);
	cvReleaseImage(&dstImage);

}

//int main()
//{
//	Mat src, src_gray;
//
//	/// Read the image
//	src = imread("IMG_0107.png", 1);
//	resize(src, src, Size(600, 600));
//	if (!src.data)
//	{
//		return -1;
//	}
//
//	/// Convert it to gray
//	cvtColor(src, src_gray, CV_BGR2GRAY);
//	GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);
//	threshold(src_gray, src_gray, 50, 255, CV_THRESH_OTSU);
//	imshow("gray", src_gray);
//	/// Reduce the noise so we avoid false circle detection
//	
//
//
//	
//	vector<Vec3f> circles;
//
//	/// Apply the Hough Transform to find the circles
//	HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 1, 50, 40, 10, 0, 50);
//
//	/// Draw the circles detected
//	for (size_t i = 0; i < circles.size(); i++)
//	{
//		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//		int radius = cvRound(circles[i][2]);
//		// circle center
//		circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//		// circle outline
//		circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
//	}
//
//	/// Show your results
//	namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
//	imshow("Hough Circle Transform Demo", src);
//
//	waitKey(0);
//	return 0;
//}

//void main(){
//	Mat myImage = imread("test3.png");
//	//resize(myImage, myImage, Size(500, 500));
//	Mat grayImage = myImage.clone();
//	cvtColor(myImage, grayImage, CV_RGB2GRAY);
//	Mat resultImage = grayImage.clone();
//
//
//	const int size = grayImage.rows * grayImage.cols;
//	double * dat_a = new double[size * 6];
//	double * dat_b = new double[size];
//	int idx = 0, count = 0;
//	
//	for (int y = 0; y < grayImage.rows; y++)
//	{
//		for (int x = 0; x < grayImage.cols; x++)
//		{
//			dat_a[idx++] = y*y;
//			dat_a[idx++] = x*x;
//			dat_a[idx++] = x*y;
//			dat_a[idx++] = y;
//			dat_a[idx++] = x;
//			dat_a[idx++] = 1;
//			//dat_b[count++] = (double)m_InImage[y][x];
//			dat_b[count++] = grayImage.data[y * grayImage.cols + x];
//		}
//	}
//	CMatrix matA(dat_a, size, 6);
//	CMatrix matB(dat_b, size, 1);
//	CMatrix pinv = (matA.transpose() * matA).inverse() * matA.transpose();
//	CMatrix matX = pinv * matB;
//	CMatrix matI = matA * matX;
//	CMatrix matR = matB - matI;
//
//
//
//	double average = 0;
//	for (int i = 0; i < size; i++)
//	{
//		average += matI.data[i];
//	}
//	average /= size;
//	count = 0;
//	for (int i = 0; i < resultImage.rows; i++)
//	{
//		for (int j = 0; j < resultImage.cols; j++)
//		{
//			//resultImage.data[i * resultImage.cols + j] = abs(grayImage.data[i * resultImage.cols + j] - matI.data[count++]);
//			int data = grayImage.data[i * resultImage.cols + j] + (average - matI.data[count++]);
//			if (data > 255) data = 255;
//			if (data < 0) data = 0;
//			resultImage.data[i * resultImage.cols + j] = data;
//
//
//		}
//	}
//
//
//	//threshold(resultImage, resultImage, 0 , 255, CV_THRESH_OTSU);
//
//	imshow("src", myImage);
//	imshow("gray", grayImage);
//	imshow("result", resultImage);
//
//	waitKey(0);
//}