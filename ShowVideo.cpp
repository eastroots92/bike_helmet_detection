

#include "ShowVideo.h"

void showVideo(String name, Mat matSrc) {
	int width = 500;
	int height = 300;
	namedWindow(name, 0);
	imshow(name, matSrc);
	resizeWindow(name, width, height);
}