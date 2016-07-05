#pragma once

#include <opencv\cv.h>
#include <opencv\highgui.h>
class RollingBall
{
public:
	RollingBall(double radius);
	~RollingBall();

	IplImage*	m_outputIpl;

	float* data;
	int width;
	int shrinkFactor;

	//	void RollingBall(double radius);

	static void subtractBackground(IplImage* input, IplImage* output, double ballRadius);
private:
	void buildRollingBall(double ballradius, int arcTrimPer);
	static void rollingBallBackground(IplImage* ip, IplImage* op, double radius, bool createBackground,
		bool lightBackground, bool useParaboloid, bool doPresmooth, bool correctCorners);
	static void rollingBallFloatBackground(IplImage* fp, float radius, bool invert, bool doPresmooth, RollingBall ball);
	static double filter3x3(IplImage* fp, int type);
	static double filter3(float* pixels, int length, int pixel0, int inc, int type);
	static void shrinkImage(IplImage* ip, IplImage* smallImage, int shrinkFactor);
	static void rollBall(RollingBall ball, IplImage* fp);
	static void enlargeImage(IplImage* smallImage, IplImage* fp, int shrinkFactor);
	static void makeInterpolationArrays(int* smallIndices, float* weights, int length, int smallLength, int shrinkFactor);

};

