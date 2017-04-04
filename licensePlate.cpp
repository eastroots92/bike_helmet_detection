
#include "licensePlate.h"

void LicensePlate::isLicensePlateDetection(Mat frame)
{
	cvtColor(frame, frame, CV_LOAD_IMAGE_GRAYSCALE);

	lpimg = &IplImage(frame);

	bin = cvCreateImage(cvGetSize(lpimg), IPL_DEPTH_8U, 1);

	cvThreshold(lpimg, bin, 100, 255, CV_THRESH_BINARY_INV);

	Mat element5(5, 5, CV_8U, Scalar(1));
	Mat element11(11, 11, CV_8U, Scalar(1));

	// 닫기 연산자 적용
	Mat closed;
	morphologyEx(cvarrToMat(bin), closed, MORPH_CLOSE, element5);

	imshow("Closed Image", closed);

	CBlobLabeling LicensePlate;

	LicensePlate.SetParam(bin, 300);
	LicensePlate.DoLabeling();

	//////// 영역 세로/가로 비율 지정////////////
	LicensePlate.BlobWidthHeightSmallRatioConstraint(1.5);
	//////// 영역 세로/가로 비율 지정////////////

	for (int i = 0; i < LicensePlate.m_nBlobs; i++) 
	{
		cvSetImageROI(bin, LicensePlate.m_recBlobs[i]);

		imgRoi = cvarrToMat(bin);

		morphologyEx(cvarrToMat(bin), closed, MORPH_CLOSE, element11);

		imshow("RoIClosed Image", closed);

		label.SetParam(&IplImage(closed), 300);
		label.DoLabeling();

		if (label.m_nBlobs > 7) {
			for (int j = 0; j < label.m_nBlobs; j++)
				rectangle(imgRoi, label.m_recBlobs[j], Scalar(255), 2);
			break;
		}
	}
}