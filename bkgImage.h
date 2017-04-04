

#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace std;
using namespace cv;


class BkgImage {
private:
	VideoCapture capture;
	Size size;
	String file;
	int fps;
	int delay;
	int frameNum;
	Mat frame;
	Mat backPicture;
	Mat grayImage;
public:
	BkgImage(){ frameNum = 0; }
	BkgImage(String file);
	void isAcummulateBkgImage();

public:
	int getFps() { return fps; }
	int getDelay() { return delay; }
	int getFrameNum() { return frameNum;}
	Mat getBackPicture() { return backPicture; }
	Mat getGrayImage() { return grayImage; }
	Size getSize() { return size; }
	String getFile() { return file; }

	void setFile(String file) { this->file = file; }
};

