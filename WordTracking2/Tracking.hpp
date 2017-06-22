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

class Tracking{
public:
    static int doTracking(cv::Mat image);
    
private:
};

#endif /* Tracking_hpp */
