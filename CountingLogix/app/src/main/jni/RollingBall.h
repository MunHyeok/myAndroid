//
// Created by CHK on 2016-07-06.
//

#ifndef COUNTINGLOGIX_ROLLINGBALL_H
#define COUNTINGLOGIX_ROLLINGBALL_H



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Ball.h"
class RollingBall
{	//	void RollingBall(double radius);
public:
	static void subtractBackground(IplImage* input, IplImage* output, double ballRadius);
private:

	static void rollingBallBackground(IplImage* ip, IplImage* op, double radius, bool createBackground,
		bool lightBackground, bool useParaboloid, bool doPresmooth, bool correctCorners);
	static void rollingBallFloatBackground(IplImage* fp, float radius, bool invert, bool doPresmooth, Ball ball);
	static double filter3x3(IplImage* fp, int type);
	static double filter3(float* pixels, int length, int pixel0, int inc, int type);
	static void shrinkImage(IplImage* ip, IplImage* smallImage, int shrinkFactor);
	static void rollBall(Ball ball, IplImage* fp);
	static void enlargeImage(IplImage* smallImage, IplImage* fp, int shrinkFactor);
	static void makeInterpolationArrays(int* smallIndices, float* weights, int length, int smallLength, int shrinkFactor);

};





#endif //COUNTINGLOGIX_ROLLINGBALL_H
