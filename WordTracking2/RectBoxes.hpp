//
//  RectBoxes.hpp
//  WordTracking2
//
//  Created by boyang on 5/10/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#ifndef RectBoxes_hpp
#define RectBoxes_hpp

#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"

class RectBoxes {
public:
    static void addCorner(CvPoint pt, int flag);
    static int getRectCornerSize(int flag);
    static std::vector<CvPoint> popFromRectCorner(int flag);
    static bool insideTheBox(CvPoint topLeft, CvPoint bottomRight, CvPoint pointPreFram);
    static void pushToInBoxPointsPreFrame(CvPoint pt);
    static void pushToInBoxPointsCurFrame(CvPoint pt);
    static void pushDiff(CvPoint pt);
    static CvPoint calculateMedianTranslationVec();
    static void shiftFromSubqueueToGlobalQueue();
    static void clearRectBoxCorners();
    static void clearVec(int flag);
    static std::vector<cv::Rect> outPutRect();
    
private:
    static std::vector<CvPoint> rectBoxCorners;
    static std::vector<CvPoint> rectBoxCornersCatchUpToCurFrame;
    static std::vector<CvPoint> pointDiff;

};

#endif /* RectBoxes_hpp */
