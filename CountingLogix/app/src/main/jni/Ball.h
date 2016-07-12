//
// Created by CHK on 2016-07-12.
//

#ifndef COUNTINGLOGIX_BALL_H
#define COUNTINGLOGIX_BALL_H



class Ball {
public:
	Ball(double radius);
	~Ball();

	float* data;
	int width;
	int shrinkFactor;
private:
    void buildRollingBall(double ballradius, int arcTrimPer);

};



#endif //COUNTINGLOGIX_BALL_H
