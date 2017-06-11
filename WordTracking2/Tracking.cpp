//
//  Tracking.cpp
//  WordTracking2
//
//  Created by boyang on 5/24/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#include "Tracking.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "stdio.h"
#include "math.h"
#include <iostream>
#include <map>
#include "RectBoxes.hpp"
#include "ReadRectFromFile.hpp"

using namespace cv;

//global var
const int MAX_CORNERS = 750;
const int TOLERENCE_WINSIZE = 5;//half of the winsize eg. 3 means winsize is 7
const int SSD_WINSIZE = 3;//half of the winsize eg. 5 means winsize is 11
const double SSD_THRESHOLD = 4;
const Size imgSize = Size(720,480);//640, 480
bool opticalFlowLineShow = false;
bool readRectFromTxt = true;
bool videoInput;

TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
Size subPixWinSize(10,10), winSize(31,31);


int cntAddByTolerance = 0;
int count = 0;
int cntTolerancePerformance = 0;
int cnt_total_valid_point = 0;


Scalar chainLengthColor[8] = {Scalar(0,0,255),Scalar(0,153,255),Scalar(0,255,255),Scalar(0,255,0),Scalar(255,255,0),Scalar(255,0,0),Scalar(255,0,153),Scalar(0,0,0)};//rainbow order + black

Mat imgPre;
Mat imgCur;
Mat imgNext;
Mat imgShow;
std::vector<cv::String> fileNames;
std::vector<std::vector<CvPoint>> featureList(MAX_CORNERS , std::vector<CvPoint>(0,0));
std::map<CvPoint , int > map;
std::vector<CvPoint> reuse2;
std::vector<std::vector<int> > trackingTable;//a table to keep a record of tracking
std::vector<Mat> preFrames(10);//store 10 frames to drag rects

//functions
//For hashmap
bool operator<(cv::Point const& a, cv::Point const& b)
{
    return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

//square function
inline static double square(int a)
{
    return a * a;
}

//Get the coordinates by mouse
void onMouse( int event, int x, int y, int, void* ) {
    if( event != CV_EVENT_LBUTTONDOWN )
        return;
    
    CvPoint pt = CvPoint(x,y);
    RectBoxes::addCorner(pt, 2);
    std::cout<<"x="<<pt.x<<"\t y="<<pt.y<<"\n";
}

CvPoint add_tolerance(int x, int y ){
    return CvPoint(x,y);
}

bool checkFeasibility (CvPoint newPoint, int i, std::vector<CvPoint> & reuse, int reuseIt, std::vector<int> & trackingTableThisFrame){
    if (map.find(newPoint) != map.end()
        && featureList[map[newPoint]].size() < i*2) {
        int index2 = map[newPoint];
        featureList[index2].push_back(reuse[reuseIt]);
        featureList[index2].push_back(reuse[reuseIt + 1]);
        cntTolerancePerformance++;
        //keep record on the tracking table
        trackingTableThisFrame[index2] = trackingTable[i - 2][index2] + 1;
        return true;
    } else {
        return false;
    }
}

//second round check:check the tracking point by adding some tolerance
void second_round_check (std::vector<CvPoint> & reuse,std::vector<CvPoint> & temp, int i, std::vector<int> & trackingTableThisFrame){
    for (int reuseIt = 0 ; reuseIt < reuse.size() ; reuseIt+=2) {
        CvPoint originalPoint = reuse[reuseIt];
        CvPoint newPoint;
        newPoint = add_tolerance(originalPoint.x + 1, originalPoint.y);
        if (checkFeasibility(newPoint, i, reuse, reuseIt, trackingTableThisFrame)){
            continue;
        }
        newPoint = add_tolerance(originalPoint.x - 1, originalPoint.y);
        if (checkFeasibility(newPoint, i, reuse, reuseIt, trackingTableThisFrame)){
            continue;
        }
        newPoint = add_tolerance(originalPoint.x, originalPoint.y + 1);
        if (checkFeasibility(newPoint, i, reuse, reuseIt, trackingTableThisFrame)){
            continue;
        }
        newPoint = add_tolerance(originalPoint.x, originalPoint.y - 1);
        if (checkFeasibility(newPoint, i, reuse, reuseIt, trackingTableThisFrame)){
            continue;
        }
        //didn't find a point to continue tracking even add 1 pixel tolerence
        //first frame no need third round check since don't have previous frame
        if(i == 1){
            temp.push_back(reuse[reuseIt]);
            temp.push_back(reuse[reuseIt + 1]);
        } else {//ready to process third round check
            reuse2.push_back(reuse[reuseIt]);
            reuse2.push_back(reuse[reuseIt + 1]);
        }
        
    }
}

bool checkOutOfBound (CvPoint thisPoint) {
    if(thisPoint.x - SSD_WINSIZE < 0
       || thisPoint.x + SSD_WINSIZE >= imgSize.width
       || thisPoint.y -SSD_WINSIZE < 0
       || thisPoint.y + SSD_WINSIZE >= imgSize.height
       ) {
        return true;
    }
    return false;
}

int ssd(CvPoint firstPoint, CvPoint secondPoint, int flag){
    Mat img1 = imgPre;
    Mat img2;
    if(flag == 1) {
        img2 = imgCur;
    } else if (flag == 2) {
        img2 = imgNext;
    }
    int sum = 0;
    if(checkOutOfBound(firstPoint) || checkOutOfBound(secondPoint)){
        return -1;
    }
    for(int y = - SSD_WINSIZE ; y <= SSD_WINSIZE ; y++) {
        for(int x = -SSD_WINSIZE ; x <= SSD_WINSIZE ; x++) {
            Scalar intensity1 = img1.at<uchar>(firstPoint.x + x, firstPoint.y + y);
            Scalar intensity2 = img2.at<uchar>(secondPoint.x + x, secondPoint.y + y);
            
            sum += square(intensity1.val[0] - intensity2.val[0]);
            //check
            //std::cout<<"1:"<<intensity1.val[0]<<" 2:"<<intensity2.val[0]<<std::endl;
        }
    }
    return sum;
}

void thirdRoundCheck(int i, std::vector<CvPoint> & temp, std::vector<int> & trackingTableThisFrame) {
    //loop through list:reuse2 which are non tracking new points. check them whether can they connect to the tracking chain
    for(int reuse2It = 0 ; reuse2It < reuse2.size() ; reuse2It+=2) {
        CvPoint startPoint = reuse2[reuse2It];//a vector's start point in current frame(imgCur)
        CvPoint endPoint = reuse2[reuse2It + 1];//a vector's end point in current frame(imgCur)
        double minRatio = DBL_MAX;//set ration to max
        int indexToBeUse = -1;//set a not valid number
        for(int dy = startPoint.y - TOLERENCE_WINSIZE ; dy <= startPoint.y + TOLERENCE_WINSIZE ; dy++ ) {
            for (int dx = startPoint.x - TOLERENCE_WINSIZE ; dx <= startPoint.x + TOLERENCE_WINSIZE ; dx++) {
                CvPoint pointWithTolerance = CvPoint(dx,dy);//assume a end point in the previous frame
                CvPoint preStartPoint;
                //if found a same end point in previous frame as pointwithTolerance(=previous end point)
                if(map.find(pointWithTolerance) != map.end()) {
                    int index3 = map[pointWithTolerance];
                    int size = featureList[index3].size();
                    //check whether already have tracking chain or not
                    if (size < i*2){
                        //check if it is the right one
                        if (featureList[index3][size - 1].x == pointWithTolerance.x
                            && featureList[index3][size - 1].y == pointWithTolerance.y) {
                            //index of the start point in the previous frame is size - 2
                            preStartPoint = featureList[index3][size - 2];//last index is size-1,want the one before the last one
                            int U_0 = ssd(preStartPoint, pointWithTolerance, 1);
                            int U_1 = ssd(preStartPoint, startPoint, 1);
                            double ratio = DBL_MAX;
                            if(U_0 == 0) {
                                if(U_1 == 0) {
                                    ratio = 1;
                                }
                            } else if (U_0 != -1 && U_1 != -1) {
                                ratio = U_1/U_0;
                            }
                            if ( ratio < SSD_THRESHOLD && ratio < minRatio) {
                                minRatio = ratio;
                                indexToBeUse = index3;
                            }
                            
                        } else {
                            std::cout<<"not find the right point in previous frame"<<std::endl;
                        }
                    }
                }
            }
        }
        //if been modified -> have good matches. check whether it already have tracking point or not
        if (minRatio != DBL_MAX && featureList[indexToBeUse].size() < i*2) {
            featureList[indexToBeUse].push_back(startPoint);
            featureList[indexToBeUse].push_back(endPoint);
            cntAddByTolerance++;
            trackingTableThisFrame[indexToBeUse] = trackingTable[i - 2][indexToBeUse] + 1;
            
        } else {
            temp.push_back(startPoint);
            temp.push_back(endPoint);
        }
    }
}

//analysis: static of tracking chain
void static_of_tracking_chain (std::vector<std::vector<CvPoint>> featureList) {
    for (int i = 0 ; i < featureList.size() ; i++) {
        int cnt_tracking_chain = 1;
        for (int j = 2 ; j < featureList[i].size() ; j+=2) {
            if (featureList[i][j].x != -1
                && (featureList[i][j].x >= featureList[i][j-1].x-1
                    && featureList[i][j].x <= featureList[i][j-1].x+1)
                && (featureList[i][j].y >= featureList[i][j-1].y-1)
                && featureList[i][j].y <= featureList[i][j-1].y+1) {
                cnt_tracking_chain++;
            } else {
                if (cnt_tracking_chain != 1){
                    std::cout<<cnt_tracking_chain<<std::endl;
                }
                cnt_tracking_chain = 1;
            }
        }
    }
}

//analysis: static of tracking chain by tracking table
void analysis() {
    for (int j = 0 ; j < trackingTable[0].size() ; j++) {
        int cntChainLength = 1;//start from 1. since one keypoint count as 1
        for (int i = 0 ; i < trackingTable.size() ; i++) {
            if(trackingTable[i][j] == 1) {
                cntChainLength++;
            } else {
                if(cntChainLength != 1){//don't output the chain length == 1
                    std::cout<<cntChainLength<<std::endl;
                }
                cntChainLength = 1;
            }
        }
    }
}

void drawRectangle(CvPoint topLeft, CvPoint bottomRight, int colorIndex) {
    Scalar color = Scalar(0, 255, 0);
    if (colorIndex == 1) {
        color = Scalar(0, 0, 255);
    }
    CvPoint topRight = CvPoint(bottomRight.x, topLeft.y);
    CvPoint bottomLeft = CvPoint(topLeft.x, bottomRight.y);
    line(imgShow,topLeft,topRight,color,2);
    line(imgShow,topRight,bottomRight,color,2);
    line(imgShow,bottomRight,bottomLeft,color,2);
    line(imgShow,bottomLeft,topLeft,color,2);
}

//find point in the rect and calculate the translation vector than create new rect
//flag==1 for keep update for current frame, flag==2 for far more previous Frame to catch up and update To CurrentFrame
void findPointInRectAndCreateNewRect(int i, int flag) {
    int rectListSize = RectBoxes::getRectCornerSize(flag);
    //loop through all the rectangles. Each rect use 2 spot to store. So r+=2
    for(int r = 0 ; r < rectListSize ; r+=2) {
        CvPoint topLeft = RectBoxes::popFromRectCorner(flag);
        CvPoint bottomRight = RectBoxes::popFromRectCorner(flag);
        //loop through all key point in previous frame
        for(int k = 0 ; k < MAX_CORNERS ; k++) {
            CvPoint pointPreFrame = featureList[k][(i-2)*2];
            //if it is a tracking point and it is in the rect box
            if(trackingTable[i-1][k] > 0 && RectBoxes::insideTheBox(topLeft, bottomRight, pointPreFrame)) {
                CvPoint correlatePointInThisFrame = featureList[k][(i-1)*2];
                CvPoint diff = CvPoint(correlatePointInThisFrame.x-pointPreFrame.x, correlatePointInThisFrame.y-pointPreFrame.y);
                RectBoxes::pushDiff(diff);
            }
        }
        CvPoint medianTranslationVec = RectBoxes::calculateMedianTranslationVec();
        // chack validity
        if(medianTranslationVec.x != -20000 && medianTranslationVec.y != -20000) {
            CvPoint newTopLeft = CvPoint(topLeft.x + medianTranslationVec.x, topLeft.y +medianTranslationVec.y);
            CvPoint newBottomRight = CvPoint(bottomRight.x + medianTranslationVec.x, bottomRight.y + medianTranslationVec.y);
            
            //update the rect for this frame
            RectBoxes::addCorner(newTopLeft, flag);
            RectBoxes::addCorner(newBottomRight, flag);
            
            //draw rectangle
            if(flag == 1) {
                drawRectangle(newTopLeft, newBottomRight, 0);
            }
        }
    }
}

void insertFrameNumAndUpdateToCurrentFrame(int i) {
    //insert the absolute frame number
    std::cout<<"Please insert frame number"<<std::endl;
    int frameNumDelayed;
    std::cin>>frameNumDelayed;
    std::cout<<"Frame Numer is:"<<frameNumDelayed<<std::endl;
    if(frameNumDelayed > i) {
        std::cout<<"frame number should be lesser than current frame number"<<std::endl;
    } else {
        //open window to drag rectangle
        Mat imgDragRect;
        if(videoInput) {
            imgDragRect = preFrames[frameNumDelayed%10];
        } else {
            imgDragRect = imread(fileNames[frameNumDelayed], IMREAD_COLOR);
        }
//        resize(imgDragRect, imgDragRect, imgSize);
        namedWindow("Please drag rectangles");
        imshow("Please drag rectangles",imgDragRect);
        //select rect box by click 2 corner(top left and bottom right)
        setMouseCallback("Please drag rectangles", onMouse, 0 );
        //wait for dragging rectangles
        cvWaitKey(0);
        //throw window and release image
        destroyWindow("Please drag rectangles");
        imgDragRect.release();
        
        //update the rect location to current frame
        //frameNumDelayed + 1 because the rect is in the correct location in this frame, next frame and further neet to process and relocate
        for(int k = frameNumDelayed + 1 ; k <= i ; k++) {
            findPointInRectAndCreateNewRect(k, 2);
        }
        //shift the rect corner to the global queue(rectBoxCorners) to keep tracking in the frame after
        RectBoxes::shiftFromSubqueueToGlobalQueue();
    }
}

void drawOriginalRect() {
    int rectListSize = RectBoxes::getRectCornerSize(2);
    for (int r = 0 ; r < rectListSize ; r += 2) {
        CvPoint topLeft = RectBoxes::popFromRectCorner(2);//get the rect
        CvPoint bottomRight = RectBoxes::popFromRectCorner(2);
        drawRectangle(topLeft, bottomRight, 1);
        RectBoxes::addCorner(topLeft, 2);//put it back
        RectBoxes::addCorner(bottomRight, 2);
    }
    
}

void loadRectAndUpdate(int frameNum) {
    ReadRectFromFile::copyRectsToQueue(frameNum);
    drawOriginalRect();
    for (int k = frameNum + 1 + 1 - ReadRectFromFile::intervalOfFrame ; k <= frameNum  ; k++) {
        findPointInRectAndCreateNewRect(k, 2);
    }
    RectBoxes::shiftFromSubqueueToGlobalQueue();
}

int Tracking::doTracking(bool videoOrImage, std::string inputPath) {
    //set input type
    videoInput = videoOrImage;
    
    VideoCapture cap(inputPath);// open the video

    //if vedio input
    if (videoInput) {
        if(!cap.isOpened()) {
            return -1;// check if we succeeded
        }
        cap >> imgNext;
//        resize(imgNext, imgNext, imgSize);
        imgNext.copyTo(imgShow);
        imgShow.copyTo(preFrames[0]);//for drawing rect on previous frames
        cvtColor(imgNext, imgNext, COLOR_BGR2GRAY);
    } else {//image input
        //read file
        String folder = inputPath;
        cv::glob(folder, fileNames);
        //load first frame
        imgNext = imread(fileNames[1], IMREAD_GRAYSCALE);
//        resize(imgNext, imgNext, imgSize);
    }
    //frame number
    int i = 0;
    //loop through all the images in the file
    while(1) {
        //frame num increase 1
        i++;
        //check finished for images input
        if(i >= fileNames.size() - 1) {
            return 0;
        }

        //Timer
        std::clock_t start;
        double duration;
        start = std::clock();
        
        //counting all output key point both good and bad one
        int keypoint_cnt = 0;
        //count the numbers of features which are in track
        int cnt_tracking_feature_each_frame = 0;
        //put the duplicate and new key point
        std::vector<CvPoint> temp;
        //new feature to track record it in the reuse array then use some tolerance to check again
        std::vector<CvPoint> reuse;
        //create tracking table for this frame
        std::vector<int> trackingTableThisFrame(MAX_CORNERS, 0);
        
        //shift image, clear and copy
        imgPre.release();
        //copy to previous frame
        imgCur.copyTo(imgPre);
        imgCur.release();
        imgNext.copyTo(imgCur);
        imgNext.release();
        
        //video input
        if (videoInput) {
            //load the frame
            cap >> imgNext;
            //check finished
            if (imgNext.empty()) {
                return 0;
            }
//            resize(imgNext, imgNext, imgSize);
            imgNext.copyTo(imgShow);
            imgShow.copyTo(preFrames[i%10]);//for drawing rect on previous frames
            cvtColor(imgNext, imgNext, COLOR_BGR2GRAY);
        } else {//image input
            //load image
            imgNext = imread(fileNames[i+1], IMREAD_GRAYSCALE);
//            resize(imgNext, imgNext, imgSize);
            //load a color image to show
            imgShow = imread(fileNames[i], IMREAD_COLOR);
//            resize(imgShow, imgShow, imgSize);
        }
        
        int corner_count=MAX_CORNERS;
        //set it later
        int win_size;
        //vector to put output feature points
        std::vector<Point2f> featuresPre(MAX_CORNERS);
        std::vector<Point2f> featuresCur(MAX_CORNERS);
        
        //find good features to track
        goodFeaturesToTrack(imgCur,
                            featuresPre,
                            MAX_CORNERS,
                            0.01,
                            5.0,
                            Mat(),
                            3,
                            0,
                            0.04
                            );
        
        cornerSubPix(imgCur,
                     featuresPre,
                     subPixWinSize,
                     Size(-1,-1),
                     termcrit
                     );
        
        //output the status and errors of feature point
        std::vector<uchar> status;
        std::vector<float> error;
        
        //optical flow
        calcOpticalFlowPyrLK(imgCur,
                             imgNext,
                             featuresPre,
                             featuresCur,
                             status,
                             error
                             );

        
        for(int j=0;j<corner_count;j++)
        {
            keypoint_cnt++;
            //start point and end point of the optical flow
            CvPoint p0=cvPoint(cvRound(featuresPre[j].x),cvRound(featuresPre[j].y));
            CvPoint p1=cvPoint(cvRound(featuresCur[j].x),cvRound(featuresCur[j].y));
            //draw line of the optical flow
            if(opticalFlowLineShow) {
                line(imgShow,p0,p1,CV_RGB(0,255,0),1);
            }
            
            //if is the first frame
            if(i == 1) {
                //not found in second frame or large error or already recorded -> mark(-1,-1)
                if(status[j]==0 || error[j]>50 || map.find(p1) != map.end())
                {
                    featureList[j].push_back(CvPoint(-1 , -1));
                    featureList[j].push_back(CvPoint(-1 , -1));
                } else {
                    //mark it in the map
                    map.emplace(p1 , j);
                    //record the feature's coordinate
                    featureList[j].push_back(p0);
                    featureList[j].push_back(p1);
                }
                //not the first frame
            } else {
                //if not found in second frame or large error->record it in the temp array first and establish the lost feature by them at the end of each frame so that the total number of featureList won't change.(consistancy)
                if(status[j]==0|| error[j]>50) {
                    temp.push_back(CvPoint(-1 , -1));
                    temp.push_back(CvPoint(-1 , -1));
                } else if (map.find(p0) != map.end()) {//if the feature coordinate match one of the feature's end point in last frame -> connect them
                    int index = map[p0];
                    //avoid duplicate point(not duplicate)
                    if(featureList[index].size() < i*2) {
                        count++;
                        cnt_tracking_feature_each_frame++;
                        featureList[index].push_back(p0);
                        featureList[index].push_back(p1);
                        //                        std::cout<<index<<"-"<<featureList[index].size() - 2<<std::endl;
                        //                        std::cout<<"x="<<p0.x<<"y="<<p0.y<<std::endl;
                        //record on the tracking table
                        trackingTableThisFrame[index] = trackingTable[i - 2][index] + 1;
                        
                    } else {//duplicate
                        temp.push_back(CvPoint(-1 , -1));
                        temp.push_back(CvPoint(-1 , -1));
                    }
                    
                } else {//new feature to track
                    //record it in the reuse array then use some tolerance to check again
                    reuse2.push_back(p0);
                    reuse2.push_back(p1);
                }
            }
        }
        
        
        if (i > 1) {
            //to speed up,canceled the second round check
            //second round add tolerance seek tracking point
//            second_round_check(reuse, temp, i, trackingTableThisFrame);
//            reuse.clear();
            
            thirdRoundCheck(i, temp, trackingTableThisFrame);
            reuse2.clear();
        }
        
        //clear map
        map.clear();
        
        //replace the lost tracking point by the temp array's point(new feature to track and some invalid points). Also renew the hash map
        int tempIt = 0;//temp iterator
        size_t tempSize = temp.size();//mark the temp size
        for (int k = 0 ; k < featureList.size() ; k++) {
            //size!=i*2 means didn't renew in this frame
            if(featureList[k].size() != i*2) {
                //sometimes we set max corner to detect, but computer didn't find so many corner feature
                if (tempIt < tempSize) {
                    //avoid duplicate
                    if(map.find(tempIt + 1) == map.end()){
                        featureList[k].push_back(temp[tempIt++]);
                        featureList[k].push_back(temp[tempIt++]);
                    } else {
                        featureList[k].push_back(CvPoint(-1 , -1));
                        featureList[k].push_back(CvPoint(-1 , -1));
                        tempIt += 2;
                    }
                    
                } else {
                    featureList[k].push_back(CvPoint(-1 , -1));
                    featureList[k].push_back(CvPoint(-1 , -1));
                }
            }
            //put the feature coordinate(not (-1,-1) one) into the map
            //and draw circles on the feature points. Color depends on the chain length
            if (featureList[k][i*2-1].x != -1 || featureList[k][i*2-1].y != -1) {
                map.emplace(featureList[k][i*2-1] , k);
                int colorIndex = trackingTableThisFrame[k]>8?8:trackingTableThisFrame[k];
                Scalar circleColor = chainLengthColor[colorIndex];
                circle(imgShow, featureList[k][i*2 - 2], 3, circleColor, 1);
            }
        }
        if (tempIt != tempSize) {
            std::cout<<"size not match:"<<tempIt<<"-"<<tempSize<<std::endl;
        }
        //check temp size
        //        std::cout<<temp.size()<<std::endl;
        
        //push back the tracking table for this frame
        trackingTable.push_back(trackingTableThisFrame);
        
        //clear
        temp.clear();
        trackingTableThisFrame.clear();
        
        
        //check the number of tracked key point
        //        std::cout<<"tracked key point:"<<cnt_tracking_feature_each_frame<<" keypointNum"<<keypoint_cnt<<std::endl;
        //check the number of valid keypoint
        //        std::cout<<"valid key point:"<<map.size()<<std::endl;
        cnt_total_valid_point += map.size();
        
        
        //For testing - search
        //    for(int i = 0 ; i < featureList.size() ; i++){
        //        for(int j = 0 ; j < featureList[i].size() ; j++){
        //            if(featureList[i][j].x == 817 && featureList[i][j].y == 549){
        //                std::cout<< "found:" <<i<<","<<j<< std::endl;
        //            }
        //        }
        //    }
        
        
        //read the rectangles from txt file. read once every interval.
        //read the privious interval's data to simulate the delay from the server.So first frame don't read data
        //frame num i start from 1, others frame num counter start from 0. For consistance, i-1.
        if(readRectFromTxt && (i-1) != 0 && (i-1)%ReadRectFromFile::intervalOfFrame == 0) {
            
            //clear privious rect boxes
            RectBoxes::clearRectBoxCorners();
            
            //load new boxes and update to current frame
            loadRectAndUpdate(i-1);
        }
        
        if(i>1) {
            //find points in the rectangle and update from provious frame to current frame
            findPointInRectAndCreateNewRect(i, 1);
        }
        
        namedWindow("LKpyr_opticalFlow");
        resize(imgShow, imgShow, imgSize);
        imshow("LKpyr_opticalFlow",imgShow);
        std::cout<<"Current Frame Number:"<<i<<std::endl<<"Press 'd' to define boxes"<<std::endl;
        
        //Output timer
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<<"Time: "<< duration <<'\n';
        
        //save image
        imwrite("/Users/boyang/workspace/BoxTracking/result/" + std::to_string(i) + ".jpg", imgShow);
        
        int key = cvWaitKey(0);
        
        //if press key "d" means want to insert delayed frame number
        if(key == 'd') {
            insertFrameNumAndUpdateToCurrentFrame(i);
        }
        
    }
    
    
    //analysis: static of tracking chain
    //static_of_tracking_chain (featureList);
    
    
    //analysis: static of tracking chain by tracking table
    //    analysis();
    
    std::cout<<"addBy1PixelTorlerance"<<cntTolerancePerformance<<std::endl;
    std::cout<<"addByTolerance"<<cntAddByTolerance<<std::endl;
    //    std::cout<<"total tracked keypoint"<<count<<std::endl;
    std::cout<<"total valid keypoint"<<cnt_total_valid_point<<std::endl;
    
    
    
    return 0;
}



