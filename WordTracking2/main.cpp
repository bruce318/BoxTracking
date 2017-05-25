//
//  main.cpp
//  WordTracking2
//
//  Created by boyang on 4/19/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#include "Tracking.hpp"
#include <string>

int main(int argc, const char * argv[]) {
    //choose a input type video or images
    bool videoInput = false;//true is vedio input, false is images input
    //path of the source
    std::string inputPath;
    if (videoInput) {
        std::string inputPath= "/Users/boyang/workspace/BoxTracking/srcVideo/IMG_7087.MOV";//a video src
    } else {
        std::string inputPath= "/Users/boyang/workspace/BoxTracking/src";//a image src
    }

    //start
    return Tracking::doTracking(videoInput, inputPath);
    
}
