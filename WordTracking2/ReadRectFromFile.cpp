//
//  ReadRectFromFile.cpp
//  WordTracking2
//
//  Created by boyang on 5/31/17.
//  Copyright © 2017 boyang. All rights reserved.
//

#include "ReadRectFromFile.hpp"
#include <fstream>

using namespace cv;

//The number of frame interval to reset the rectangle coordinate by server
int ReadRectFromFile::intervalOfFrame = 5;

//Top left and the bottom right of the rectangle
std::vector<std::vector<CvPoint> > ReadRectFromFile::rects;

//Translate the string in a line to an integer vector
std::vector<int> stringLine2IntVector(std::string line) {
    std::vector<int> thisLine;
    int start = 0;
    int numIndex = 0;//The ordinal number in a line
    for (int end = 0 ; end < line.length(); end++) {
        int intNum = -1;
        if (line.at(end) != ' '){
            continue;
        } else {
            intNum = stoi(line.substr(start, end - start));//Get the number and convert to int
            numIndex++;//ordinal number of this number
            start = end + 1;
        }
        //If it is the first number in the line and if the frame number is a multiple of the interval number means this frame's coordinates of the rectangle need to be read and udate. Others frame don't need to be read
        if ((numIndex == 1) && (intNum%ReadRectFromFile::intervalOfFrame != 0)) {
            return thisLine;
        } else  {//the first int is a multiple of the interval num, then push back all the int in this line
            thisLine.push_back(intNum);
        }
    }
    //There is no " " at end of the line. So the last number is getten out of the loop
   thisLine.push_back(stoi(line.substr(start)));
    return thisLine;
}

//Insert the integer to the table by a kind of order
void ReadRectFromFile::loadIntoTable(std::string line) {
    std::vector<int> intInLine = stringLine2IntVector(line);

    if (intInLine.size() != 0) {
        //check the input is right
        if (intInLine.size() != 5) {
            std::cout<<"inout error: Each line should have 5 numbers"<<std::endl;
        } else {
            int frameNum = intInLine[0];
            int x = intInLine[1];
            int y = intInLine[2];
            int width = intInLine[3];
            int height = intInLine[4];
            
            int rowNum = frameNum/intervalOfFrame;
            
            //If this is the first time to insert number in this line, need to create a new line
            if ((int(rects.size()) - 1) < rowNum) {//rect.size()-1 is the row number
                std::vector<CvPoint> newLine;
                rects.push_back(newLine);
            } else {//put top left and bottom right into the vector
                rects[rowNum].push_back(CvPoint(x,y));
                rects[rowNum].push_back(CvPoint(x + width, y + height));
            }
        }
    }
}

//Transfer the coordinate of rectangle in txt file into a well ordered table
void ReadRectFromFile::loadRects(std::string rectFilePath) {
    std::string line;
    std::ifstream myfile (rectFilePath);
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
//            std::cout << line <<std::endl;
            loadIntoTable(line);
        }
        myfile.close();
    }
    else std::cout << "Unable to open file"<<std::endl;
}

void ReadRectFromFile::copyRectsToQueue(int frameNum) {
    int rowNum = frameNum/intervalOfFrame - 1;//load the privious interval's frame to simulate delay
    for (int k = 0 ; k < rects[rowNum].size() ; k++) {
        RectBoxes::addCorner(rects[rowNum][k], 2);
    }
    
}

