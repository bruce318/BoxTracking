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
    std::string filePath;
    if (argc != 2) {
        if (videoInput) {
            inputPath= "/Users/boyang/workspace/BoxTracking/videoInput/1.mp4";//a video src
        } else {
            inputPath= "/Users/boyang/workspace/BoxTracking/imageInput/1/";//a image src
        }
        filePath = "/Users/boyang/workspace/BoxTracking/boxCoordinates/1.txt";
    } else {
        inputPath = argv[0];
        filePath = argv[1];
    }
    
    
    //load rectangle box's coordinates
    ReadRectFromFile::loadRects(filePath);
    
    //start
    return Tracking::doTracking(videoInput, inputPath);

}
