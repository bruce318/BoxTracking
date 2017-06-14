#PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#export PKG_CONFIG_PATH

all:
	g++ -o ./bin/textTracking ./WordTracking2/main.cpp ./WordTracking2/Tracking.cpp ./WordTracking2/ReadRectFromFile.cpp ./WordTracking2/RectBoxes.cpp `pkg-config --cflags --libs opencv` 

