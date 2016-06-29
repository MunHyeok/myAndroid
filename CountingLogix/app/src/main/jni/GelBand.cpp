//
// Created by CHK on 2016-03-22.
//
#include "GelBand.h"

int		CGelBand::s_winSize		= 3;		// local maximum finding의 window size
float	CGelBand::s_minDiff		= 1.0f;		// 윈도우 내의 최대 밝기 차
float	CGelBand::s_minHeight	= 50.f;		// 최저 밝기
float	CGelBand::s_minRate		= 1.0f;		// 평균과의 비율 : 최저 밝기
