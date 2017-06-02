//
//  ReadRectFromFile.hpp
//  WordTracking2
//
//  Created by boyang on 5/31/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#ifndef ReadRectFromFile_hpp
#define ReadRectFromFile_hpp

#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"


using namespace cv;

class ReadRectFromFile {
public:
    static void loadRects(std::string rectFilePath);
    
private:
    static std::vector<std::vector<CvPoint> > rects;
};

#endif /* ReadRectFromFile_hpp */
