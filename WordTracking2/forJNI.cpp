//
//  forJNI.cpp
//  WordTracking2
//
//  Created by boyang on 6/22/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#include "forJNI.hpp"
#include <string>
#include "vector"
#include "Tracking.hpp"
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
    
    
    
}
