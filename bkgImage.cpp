#include "bkgImage.h"

BkgImage::BkgImage(String file)
{
	this->file = file;
	frameNum = 0;
}

void BkgImage::isAcummulateBkgImage()
{
	capture.open(file);

	if (!capture.isOpened()) {
		cout << "비디오 파일이 없음" << endl;
		return;
	}

	size = Size((int)capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT));

	fps = (int)(capture.get(CAP_PROP_FPS));

	delay = 100 / fps;

	Mat sumImage(size, CV_32F, Scalar::all(0));

	while (capture.isOpened()) {

		capture >> frame;

		if (frame.empty()) break;

		if (waitKey(1) == 27)  // 27 = Esc
			break;

		cvtColor(frame, grayImage, COLOR_BGR2GRAY);
		accumulate(grayImage, sumImage);

		frameNum++;
		int ckey = waitKey(delay);
		if (ckey == 27) break;
	}

	sumImage = sumImage / (float)frameNum;

	imwrite("BkgImage.jpg", sumImage);

	backPicture = imread("BkgImage.jpg");
}