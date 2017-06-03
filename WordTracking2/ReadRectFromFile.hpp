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
#include "RectBoxes.hpp"


using namespace cv;

class ReadRectFromFile {
public:
    static void loadRects(std::string rectFilePath);
    static void loadIntoTable(std::string line);
    static void copyRectsToQueue(int frameNum);
    static int intervalOfFrame;
    
private:
    static std::vector<std::vector<CvPoint> > rects;//A list of rectangle coordinates.
                                                    //Each row represent a frame. Inside the row is topleft(rect1),bottomright(rect1),topleft(rect2),bottomright(rect2)......
                                                    //Not every frame is nesessary. Row number = frame number/intervalOfFrame
};

#endif /* ReadRectFromFile_hpp */
