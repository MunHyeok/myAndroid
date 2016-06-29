//
// Created by CHK on 2016-03-22.
//

#ifndef ANDROID_GELBAND_H
#define ANDROID_GELBAND_H
#include "cimUtils.h"

const int GelMaxBands = 1024;	// 한 Lane에서의 최대 Band의 개수

class CGelBand
{
    float		m_peak;			// band의 peak 위치
    int			m_minY;			// band의 시작 위치
    int			m_maxY;			// band의 끝 위치
    int			m_width;		// band의 폭
    float		m_peakGray;		// 최대 밝기 (peak 위치)
    float		m_avgGray;		// 평균 밝기 (band 폭 내의 평균)

    friend class CGelLane;
public:
    CGelBand(void)	{}
    ~CGelBand(void)	{}

    static int		s_winSize;		// local maximum finding의 window size
    static float	s_minDiff;		// 윈도우 내의 최대 밝기 차
    static float	s_minHeight;	// 밴드 최저 발기
    static float	s_minRate;		// 평균과의 비율 : 최저 밝기


    static int 	FindAllBands( float *prj, int len, float *band ) {
        float	mean, stdDev;
        cimCalcAvgSdv( prj, len, mean, stdDev );
        float	thMinHeight = cvMAX( mean*s_minRate, s_minHeight );

        int nBands = 0;
        for( int i=s_winSize ; i<len-s_winSize ; i++ ) {
            if( cimIsLocalMax( prj, len, i, s_winSize ) ) {
                if( prj[i] >= thMinHeight ) {
                    band[nBands] = cimSubPixelPosi( prj, i );

                    nBands++;
                }
                /*
                else {
                    float diff = cimLocalMaxDiff( prj, len, i, s_winSize );
                    if( diff > s_minDiff )
                        band[nBands++] = cimSubPixelPosi( prj, i );
                }
                */
            }
        }
        return nBands;
    }
};


#endif //ANDROID_GELBAND_H
