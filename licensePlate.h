
#include "blob.h"
#include <iostream>

using namespace std;
using namespace cv;

class LicensePlate
{
private:
	CBlobLabeling label;
	IplImage * lpimg;
	IplImage * bin;
	Mat imgRoi;

public:
	LicensePlate() {}
	void isLicensePlateDetection(Mat frame);

public:
	CBlobLabeling getLabel() {return label;}
	IplImage * getLpimg() { return lpimg; }
	IplImage * getBin() { return  bin; }
	Mat getImgRoi() { return imgRoi; }

};


