# Text Tracking #
This project built a tracker to track all the text on the screen. To test the performance of this tracker, there is 2 testing video in the folder "videoInput" for input. And the corresponding text location data is in the folder "boxCoordinates". The system read the text location data every 5 frames. But it reads the text location which was 5 frames before the current frame. By tracking, the text location (represented by the bounding box) is updated to the current frame. And the system keeps tracking the text and until reading the data again which is 5 frames later. 
## What's in the folder ##
* bin -- textTracking.exe
* boxCoordinates -- corresponding text location data for each video input
* result -- the folder to store the result
* videoInput -- 2 testing video input inside to test the code
* WordTracking2 -- codes

## Prerequisites ##
* opencv 3.1.0

## Running the code ##
The software is compiled using the Makefile. Simply run 

	$make

to compile.

Then run 

    $ ./bin/textTracking [video input path] [text location data path]

For example: 

    $ ./bin/textTracking ./videoInput/1.mp4 ./boxCoordinates/1.txt

If video input path is blank, the system will run the video 1 by default.
## Commands ##
When running the code:

* 'q' -- quit
* 'n' -- next frame 

## The Result ##

The bounding boxes represent the text field. The red boxes are the original text location which read directly from the file(text location of 5 frames before the current frame). The green boxes are the text field updated to the current frame by using tracking technique. The white line connected the original bounding box with the updated one.

