//
//  forJNI.cpp
//  WordTracking2
//
//  Created by boyang on 6/22/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#include "forJNI.hpp"
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include <string>
#include "vector"
#include "Tracking.hpp"
#include "RectBoxes.hpp"
#include "../../../../../../../../../AppData/Local/Android/sdk/ndk-bundle/platforms/android-21/arch-mips/usr/include/jni.h"

std::vector<int> nums;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_leo_neat_ctest_MainActivity_stringFromJNI(
                                                   JNIEnv *env,
                                                   jobject /* this */,
                                                   jint jnum) {
    //    int num = (int) jnum;
    //    nums.push_back(num);
    //    std::string result;
    //    for (int i = 0 ; i < nums.size() ; i++) {
    //        result = result + std::to_string(nums[i]);
    //    }
    //    return env->NewStringUTF(result.c_str());
    
    Tracking::doTracking(image);
    
    // the rectangles are already updated but with delay?
    if (recieve update from server)  {
        //clear privious rect boxes
        RectBoxes::clearRectBoxCorners();
        //get each rectangle and update
        for (Rect rect:rects) {//loop through all the rectangle
            //Put the rectangles to the buffer list
            ReadRectFromFile::putRectIntoList(Rect rect);
        }
        //Catch up to current frame
        Tracking::insertFrameNumAndUpdateToCurrentFrame(frameNumDelayed, frameNum);
    }
    
    if(i>1) {
            //find points in the rectangle and update from provious frame to current frame
        Tracking::findPointInRectAndCreateNewRect(frameNum, 1);
    }
    
    //output the updated rectangles
    std::vector<cv::Rect> result;
    result = RectBoxes::outPutRect();
    
    


    
    
}
