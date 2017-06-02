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
int intervalOfFrame = 5;

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
        if ((numIndex == 1) && (intNum%intervalOfFrame != 0)) {
            return thisLine;
        } else if (numIndex == 1) {//pushback first num
            thisLine.push_back(intNum);
        } else if (numIndex == 2) {//pushback 2ed num
            thisLine.push_back(intNum);
        } else if (numIndex == 3) {//pushback 3rd num
            thisLine.push_back(intNum);
        } else if (numIndex == 4) {//pushback 4th num
            thisLine.push_back(intNum);
        }
    }
    //There is no " " at end of the line. So the last number is getten out of the loop
   thisLine.push_back(stoi(line.substr(start)));
    return thisLine;
}

//Insert the integer to the table by a kind of order
void loadIntoTable(std::string line) {
    std::vector<int> intInLine = stringLine2IntVector(line);
    for (int i = 0 ; i < intInLine.size() ; i ++) {
        std::cout << intInLine[i] <<std::endl;
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
