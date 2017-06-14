//
//  RectBoxes.cpp
//  WordTracking2
//
//  Created by boyang on 5/10/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#include "RectBoxes.hpp"

std::vector<CvPoint> RectBoxes::rectBoxCorners;
std::vector<CvPoint> RectBoxes::rectBoxCornersCatchUpToCurFrame;
std::vector<CvPoint> RectBoxes::pointDiff;

struct mySortClass {
    bool operator() (CvPoint pt1, CvPoint pt2) {
        if(pt1.x == pt2.x) {
            return (pt1.y < pt2.y);
        }
        return (pt1.x < pt2.x);
    }
} mySort;

//add rectangle's corners to the list(top left and bottom right corner)
//flag==1 for update from current flame to next frame, flag==2 for update from far behind to catch up to current flame
void RectBoxes::addCorner(CvPoint pt, int flag) {
    if(flag == 1) {
        rectBoxCorners.push_back(pt);
    } else if (flag == 2) {
        rectBoxCornersCatchUpToCurFrame.push_back(pt);
    }
//    std::cout<<"rectBoxCorners Size:"<<rectBoxCorners.size()<<std::endl;
}

//get current size of the queue(rectBoxCorners)
int RectBoxes::getRectCornerSize(int flag) {
    if (flag == 1) {
        return rectBoxCorners.size();
    } else if (flag == 2) {
        return rectBoxCornersCatchUpToCurFrame.size();
    }
    return -1;
}

std::vector<CvPoint> RectBoxes::popFromRectCorner(int flag) {
    std::vector<CvPoint> pts;
    if (flag == 1) {
        return rectBoxCorners;
    }
    else if (flag == 2) {
        return rectBoxCornersCatchUpToCurFrame;
    }
    return pts;
}

//check the point is inside the box or not
bool RectBoxes::insideTheBox(CvPoint topLeft, CvPoint bottomRight, CvPoint pointPreFram) {
    if(pointPreFram.x >= topLeft.x && pointPreFram.x <= bottomRight.x
       && pointPreFram.y >= topLeft.y && pointPreFram.y <=bottomRight.y) {
        return true;
    } else {
        return false;
    }
}

//put the tranlation vector to the list
void RectBoxes::pushDiff(CvPoint pt) {
    pointDiff.push_back(pt);
}

CvPoint RectBoxes::calculateMedianTranslationVec() {
    int size = pointDiff.size();
    if (size < 2) {
        std::cout<<"Not enough features found in box"<<std::endl;
        return CvPoint(-20000,-20000);
    }
    std::sort(pointDiff.begin(), pointDiff.end(), mySort);
    CvPoint ans = pointDiff[size/2];
    pointDiff.clear();
    return ans;
}

void RectBoxes::shiftFromSubqueueToGlobalQueue() {
    rectBoxCorners.clear();
    for (int k = 0 ; k < rectBoxCornersCatchUpToCurFrame.size() ; k++) {
        rectBoxCorners.push_back(rectBoxCornersCatchUpToCurFrame[k]);
    }
    rectBoxCornersCatchUpToCurFrame.clear();
}

void RectBoxes::clearRectBoxCorners() {
    rectBoxCorners.clear();
}

void RectBoxes::clearVec(int flag) {
    if (flag == 1) {
        rectBoxCorners.clear();
    }
    else if (flag == 2) {
        rectBoxCornersCatchUpToCurFrame.clear();
    }
}
