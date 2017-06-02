//
//  main.cpp
//  WordTracking2
//
//  Created by boyang on 4/19/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#include "Tracking.hpp"
#include "ReadRectFromFile.hpp"
#include <string>

int main(int argc, const char * argv[]) {
    //choose a input type video or images
    bool videoInput = true;//true is vedio input, false is images input
    //path of the source
    std::string inputPath;
    if (videoInput) {
        inputPath= "/Users/boyang/workspace/BoxTracking/videoInput/Video_30_2_3.mp4";//a video src
    } else {
        inputPath= "/Users/boyang/workspace/BoxTracking/imageInput2/";//a image src
    }
    //load rectangle box's coordinates
    ReadRectFromFile::loadRects("/Users/boyang/workspace/BoxTracking/boxCoordinates/Video_5_3_2.txt");
    
    //start
    return Tracking::doTracking(videoInput, inputPath);

}
