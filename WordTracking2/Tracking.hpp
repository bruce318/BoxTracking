//
//  Tracking.hpp
//  WordTracking2
//
//  Created by boyang on 5/24/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#ifndef Tracking_hpp
#define Tracking_hpp

#include <stdio.h>
#include <string>
#include "opencv2/core/core.hpp"

using namespace cv;

class Tracking{
public:
    static int doTracking(Mat image);
    static void insertFrameNumAndUpdateToCurrentFrame(int frameNumDelayed, int frameNum);
    static void findPointInRectAndCreateNewRect(int i, int flag);
private:
};

#endif /* Tracking_hpp */
