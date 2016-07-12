//
// Created by CHK on 2016-07-12.
//

#include "Ball.h"
#include <math.h>
Ball::Ball(double radius)
{
	int arcTrimPer;
	if (radius <= 10) {
		shrinkFactor = 1;
		arcTrimPer = 24; // trim 24% in x and y
	}
	else if (radius <= 30) {
		shrinkFactor = 2;
		arcTrimPer = 24; // trim 24% in x and y
	}
	else if (radius <= 100) {
		shrinkFactor = 4;
		arcTrimPer = 32; // trim 32% in x and y
	}
	else {
		shrinkFactor = 8;
		arcTrimPer = 40; // trim 40% in x and y
	}
	buildRollingBall(radius, arcTrimPer);
}

Ball::~Ball()
{
}

/** Computes the location of each point on the rolling ball patch relative to the
center of the sphere containing it.  The patch is located in the top half
of this sphere.  The vertical axis of the sphere passes through the center of
the patch.  The projection of the patch in the xy-plane below is a square.
*/
void Ball::buildRollingBall(double ballradius, int arcTrimPer) {
	double rsquare;     // rolling ball radius squared
	int xtrim;          // # of pixels trimmed off each end of ball to make patch
	int xval, yval;     // x,y-values on patch relative to center of rolling ball
	double smallballradius; // radius of rolling ball (downscaled in x,y and z when image is shrunk)
	int halfWidth;      // distance in x or y from center of patch to any edge (patch "radius")

	//	this.shrinkFactor = shrinkFactor;
	smallballradius = ballradius / shrinkFactor;
	if (smallballradius<1)
		smallballradius = 1;
	rsquare = smallballradius*smallballradius;
	xtrim = (int)(arcTrimPer*smallballradius) / 100; // only use a patch of the rolling ball
	halfWidth = (int)round(smallballradius - xtrim);
	width = 2 * halfWidth + 1;
	data = new float[width*width];

	for (int y = 0, p = 0; y<width; y++)
	for (int x = 0; x<width; x++, p++) {
		xval = x - halfWidth;
		yval = y - halfWidth;
		double temp = rsquare - xval*xval - yval*yval;
		data[p] = temp>0. ? (float)(sqrt(temp)) : 0.0;
		//-Float.MAX_VALUE might be better than 0f, but gives different results than earlier versions

		//		CString Str;
		//		Str.Format(_T("%f"), data[p]);
		//		AfxMessageBox(Str);

	}
	//IJ.log(ballradius+"\t"+smallballradius+"\t"+width); //###
	//IJ.log("half patch width="+halfWidth+", size="+data.length);
}
