#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2\video\background_segm.hpp>
#include "bkgImage.h"
#include "helmetDetection.h"
#include "blob.h"
#include "ShowVideo.h"
#include "licensePlate.h"

using namespace std;
using namespace cv;

class Motosycle
{
private:
	VideoCapture capture;
	String file;
	Mat frame;
	Mat bkgImage;
	Mat grayImage;
	CBlobLabeling label;
	BkgImage bkg; 

public:
	Motosycle() {  }
	Motosycle(String file) { this->file = file; }
	void isMotosycleDetection();
public:
	Mat getBackPicture() { return bkgImage;}
	Mat getGrayImage() { return grayImage; }
	String getFile() { return file; }
	CBlobLabeling getLabel(){ return label; }

	void setFile(String file) { this->file = file; }
};
