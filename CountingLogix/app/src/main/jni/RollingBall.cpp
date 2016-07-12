//
// Created by CHK on 2016-07-06.
//

#include "RollingBall.h"
#include <math.h>

void RollingBall::subtractBackground(IplImage* ip, IplImage* op, double ballRadius)
{
	rollingBallBackground(ip, op, ballRadius, false, true, true, true, true);
}

void RollingBall::rollingBallBackground(IplImage* ip, IplImage* op, double radius, bool createBackground,
	bool lightBackground, bool useParaboloid, bool doPresmooth, bool correctCorners) {

	bool invert = lightBackground;

	Ball ball(radius);//Step b
	IplImage* fp = cvCreateImage(cvSize(ip->width, ip->height), IPL_DEPTH_32F, 1); //null;

	int width = ip->width;
	int height = ip->height;
	int imagesize = width*height;
	float* fpixel = (float *)fp->imageData;
	uchar* ipixel = (uchar *)ip->imageData;

	for (int i = 0; i<imagesize; i++) fpixel[i] = (float)ipixel[i];


	rollingBallFloatBackground(fp, (float)radius, invert, doPresmooth, ball);

	//subtract the background now
	float* bgPixels = (float*)fp->imageData; //currently holds the background
	uchar* pixels = (uchar*)op->imageData; // .getPixels();
	float offset = invert ? 255.5 : 0.5;  //includes 0.5 for rounding when converting float to byte

	if (createBackground){
		for (int p = 0; p<imagesize; p++) {
			float value = bgPixels[p];
			if (value<0.0) value = 0.0;
			if (value>255.0) value = 255.0;
			pixels[p] = (uchar)(value);
		}
	}
	else{
		for (int p = 0; p<imagesize; p++) {
			float value = (ipixel[p] & 0xff) - bgPixels[p] + offset;
			if (value<0.0) value = 0.0;
			if (value>255.0) value = 255.0;

			pixels[p] = (uchar)(value);
		}
	}
}

void RollingBall::rollingBallFloatBackground(IplImage* fp, float radius, bool invert,
	bool doPresmooth, Ball ball) {

	IplImage* smallImage;
	float* pixels = (float *)fp->imageData;    // .getPixels();   //this will become the background
	bool shrink = ball.shrinkFactor > 1;
	int ImageSize = fp->width * fp->height;
	int shrinkFactor = ball.shrinkFactor;
	int sWidth, sHeight;

	if (invert)//Step C
	for (int i = 0; i<ImageSize; i++)
		pixels[i] = -pixels[i];

	if (doPresmooth)//Step D
		filter3x3(fp, 0);

	if (shrink)	{//Step E
		sWidth = (fp->width + shrinkFactor - 1) / shrinkFactor;
		sHeight = (fp->height + shrinkFactor - 1) / shrinkFactor;
	}
	else {
		sWidth = fp->width;
		sHeight = fp->height;
	}

	smallImage = cvCreateImage(cvSize(sWidth, sHeight), IPL_DEPTH_32F, 1);

	float* spixels = (float *)smallImage->imageData;

	if (shrink) shrinkImage(fp, smallImage, ball.shrinkFactor);
	else {
		for (int i = 0; i<ImageSize; i++)
			spixels[i] = pixels[i];
	}

	rollBall(ball, smallImage); // Step F

	if (shrink) enlargeImage(smallImage, fp, ball.shrinkFactor); //step G

	if (invert)// step H
	for (int i = 0; i<ImageSize; i++)
		pixels[i] = -pixels[i];
}

double RollingBall::filter3x3(IplImage* fp, int type) {
	int width = fp->width;
	int height = fp->height;
	double shiftBy = 0;
	float* pixels = (float *)fp->imageData;

	for (int y = 0; y<height; y++)
		shiftBy += filter3(pixels, width, y*width, 1, type);
	for (int x = 0; x<width; x++)
		shiftBy += filter3(pixels, height, x, width, type);
	return shiftBy / width / height;
}

double RollingBall::filter3(float* pixels, int length, int pixel0, int inc, int type) {
	double shiftBy = 0;
	float v3 = pixels[pixel0];  //will be pixel[i+1]
	float v2 = v3;              //will be pixel[i]
	float v1;                   //will be pixel[i-1]
	for (int i = 0, p = pixel0; i<length; i++, p += inc) {
		v1 = v2;
		v2 = v3;
		if (i<length - 1) v3 = pixels[p + inc];
		if (type == 65535) {
			float max = v1 > v3 ? v1 : v3;
			if (v2 > max) max = v2;
			shiftBy += max - v2;
			pixels[p] = max;
		}
		else
			pixels[p] = (v1 + v2 + v3)*0.33333333;
	}
	return shiftBy;
}

void RollingBall::shrinkImage(IplImage* ip, IplImage* smallImage, int shrinkFactor) {
	int width = ip->width;
	int height = ip->height;
	float* pixels = (float *)ip->imageData;

	int sWidth = (width + shrinkFactor - 1) / shrinkFactor;
	int sHeight = (height + shrinkFactor - 1) / shrinkFactor;

	float* sPixels = (float *)smallImage->imageData;
	float min, thispixel;

	for (int ySmall = 0; ySmall<sHeight; ySmall++) {
		for (int xSmall = 0; xSmall<sWidth; xSmall++) {
			min = 65535.0;
			for (int j = 0, y = shrinkFactor*ySmall; j<shrinkFactor&&y<height; j++, y++) {
				for (int k = 0, x = shrinkFactor*xSmall; k<shrinkFactor&&x<width; k++, x++) {
					thispixel = pixels[x + y*width];
					if (thispixel<min)
						min = thispixel;
				}
			}
			sPixels[xSmall + ySmall*sWidth] = min; // each point in small image is minimum of its neighborhood
		}
	}
}

void RollingBall::rollBall(Ball ball, IplImage* fp) {
	float* pixels = (float*)fp->imageData;  //the input pixels
	int width = fp->width;
	int height = fp->height;
	float* zBall = ball.data;
	int ballWidth = ball.width;
	int radius = ballWidth / 2;
	float* cache = new float[width*ballWidth]; //temporarily stores the pixels we work on

	for (int y = -radius; y<height + radius; y++) { //for all positions of the ball center:
		int nextLineToWriteInCache = (y + radius) % ballWidth;
		int nextLineToRead = y + radius;        //line of the input not touched yet
		if (nextLineToRead<height) {
			for (int i = 0; i < width; i++)
				cache[i + nextLineToWriteInCache*width] = pixels[i + nextLineToRead*width];
			for (int x = 0, p = nextLineToRead*width; x < width; x++, p++)
				pixels[p] = -65535.0; // -Float.MAX_VALUE;   //unprocessed pixels start at minus infinity
		}
		int y0 = y - radius;                      //the first line to see whether the ball touches
		if (y0 < 0) y0 = 0;
		int yBall0 = y0 - y + radius;               //y coordinate in the ball corresponding to y0
		int yend = y + radius;                    //the last line to see whether the ball touches
		if (yend >= height) yend = height - 1;
		for (int x = -radius; x<width + radius; x++) {
			float z = 65535.0;  //Float.MAX_VALUE;          //the height of the ball (ball is in position x,y)
			int x0 = x - radius;
			if (x0 < 0) x0 = 0;
			int xBall0 = x0 - x + radius;
			int xend = x + radius;
			if (xend >= width) xend = width - 1;
			for (int yp = y0, yBall = yBall0; yp <= yend; yp++, yBall++) { //for all points inside the ball
				int cachePointer = (yp%ballWidth)*width + x0;
				for (int xp = x0, bp = xBall0 + yBall*ballWidth; xp <= xend; xp++, cachePointer++, bp++) {
					float zReduced = cache[cachePointer] - zBall[bp];
					if (z > zReduced)           //does this point imply a greater height?
						z = zReduced;
				}
			}
			for (int yp = y0, yBall = yBall0; yp <= yend; yp++, yBall++) //raise pixels to ball surface
			for (int xp = x0, p = xp + yp*width, bp = xBall0 + yBall*ballWidth; xp <= xend; xp++, p++, bp++) {
				float zMin = z + zBall[bp];
				if (pixels[p] < zMin)
					pixels[p] = zMin;
			}
		}
	}
}

void RollingBall::enlargeImage(IplImage* smallImage, IplImage* fp, int shrinkFactor) {
	int width = fp->width;
	int height = fp->height;
	int smallWidth = smallImage->width;
	int smallHeight = smallImage->height;
	float* pixels = (float*)fp->imageData;
	float* sPixels = (float*)smallImage->imageData;
	int* xSmallIndices = new int[width];         //index of first point in smallImage
	float* xWeights = new float[width];        //weight of this point

	makeInterpolationArrays(xSmallIndices, xWeights, width, smallWidth, shrinkFactor);

	int* ySmallIndices = new int[height];
	float* yWeights = new float[height];

	makeInterpolationArrays(ySmallIndices, yWeights, height, smallHeight, shrinkFactor);

	float* line0 = new float[width];
	float* line1 = new float[width];
	for (int x = 0; x<width; x++)
		line1[x] = sPixels[xSmallIndices[x]] * xWeights[x] + sPixels[xSmallIndices[x] + 1] * (1.0 - xWeights[x]);
	int ySmallLine0 = -1;                       //line0 corresponds to this y of smallImage
	for (int y = 0; y<height; y++) {
		if (ySmallLine0 < ySmallIndices[y]) {
			float* swap = line0;               //previous line1 -> line0
			line0 = line1;
			line1 = swap;                       //keep the other array for filling with new data
			ySmallLine0++;
			int sYPointer = (ySmallIndices[y] + 1)*smallWidth; //points to line0 + 1 in smallImage
			for (int x = 0; x<width; x++)         //x-interpolation of the new smallImage line -> line1
				line1[x] = sPixels[sYPointer + xSmallIndices[x]] * xWeights[x] +
				sPixels[sYPointer + xSmallIndices[x] + 1] * (1.0 - xWeights[x]);
		}
		float weight = yWeights[y];
		for (int x = 0, p = y*width; x<width; x++, p++)
			pixels[p] = line0[x] * weight + line1[x] * (1.0 - weight);
	}
}

void RollingBall::makeInterpolationArrays(int* smallIndices, float* weights, int length, int smallLength, int shrinkFactor) {
	for (int i = 0; i<length; i++) {
		int smallIndex = (i - shrinkFactor / 2) / shrinkFactor;
		if (smallIndex >= smallLength - 1) smallIndex = smallLength - 2;
		smallIndices[i] = smallIndex;
		float distance = (i + 0.5f) / shrinkFactor - (smallIndex + 0.5f); //distance of pixel centers (in smallImage pixels)
		weights[i] = 1.0 - distance;
		//if(i<12)IJ.log("i,sI="+i+","+smallIndex+", weight="+weights[i]);
	}
}