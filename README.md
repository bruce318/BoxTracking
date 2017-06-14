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




make

./bin/textTracking ./videoInput/1.mp4 ./boxCoordinates/1.txt