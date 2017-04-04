#include "motorcycle.h"

void Motosycle::isMotosycleDetection()
{

	capture.open(file);

	if (!capture.isOpened())
	{
		cout << "비디오 파일이 없음" << endl;
		return;
	}

	bkg.setFile(file);

	bkg.isAcummulateBkgImage();

	Size size = bkg.getSize();

	int fps = bkg.getFps();

	bkgImage = imread("BkgImage.jpg", IMREAD_GRAYSCALE);

	if (bkgImage.empty()) return;

	int nTh = 35;
	int frameNum = 0;
	int delay = 100 / fps;
	Mat frame, grayImage, diffImage;
	Mat element(5, 5, CV_8U, Scalar(1));
	Mat element1(50, 50, CV_8U, Scalar(1));

	while (true)
	{
		capture >> frame;
		if (frame.empty()) break;

		cvtColor(frame, grayImage, COLOR_BGR2GRAY);

		IplImage *rect = &IplImage(grayImage);

		int rWidth = rect->width;	//rect의 가로길이
		int rHeight = rect->height;	//rect의 세로길이
		Rect rt(rWidth * 1 / 6, rHeight * 1 / 4, rWidth * 2 / 3, rHeight * 1 / 2);
		CvPoint pt1 = cvPoint(rWidth * 1 / 6, rHeight * 1 / 4);
		CvPoint pt2 = cvPoint(rWidth * 2 / 3, rHeight * 1 / 2);
		//Rect rt(가로 기준점, 세로 기준점, 너비, 높이)

		rectangle(grayImage, rt, Scalar(255, 0, 0), 2); //작업할 영역

		if (frameNum % 10 == 0)
		{
			absdiff(grayImage, bkgImage, diffImage);

			threshold(diffImage, diffImage, nTh, 255, THRESH_BINARY);

			morphologyEx(diffImage, diffImage, MORPH_OPEN, element);

			morphologyEx(diffImage, diffImage, MORPH_CLOSE, element1);

			IplImage *trans = &IplImage(diffImage);

			label.SetParam(trans, 8000);
			label.DoLabeling();
			label.BlobWidthHeightSmallRatioConstraint(1.2);

			IplImage *cycle = &IplImage(diffImage);

			for (int i = 0; i < label.m_nBlobs; i++)
			{
				label.m_recBlobs[i].y = label.m_recBlobs[i].y - 10;
				label.m_recBlobs[i].height = label.m_recBlobs[i].height + 10;
				rectangle(grayImage, label.m_recBlobs[i], Scalar(255), 2);
				cvSetImageROI(cycle, label.m_recBlobs[i]);

				int cWidth = cycle->width;
				int cHeight = cycle->height;

				Mat autoCycle = cvarrToMat(cycle);

				CvPoint cPt = cvPoint(label.m_recBlobs[i].x, label.m_recBlobs[i].y);

				CvPoint cCenter = cvPoint(cPt.x + (cWidth * (1 / 2)), cPt.y + (cHeight * (1 / 2)));

				if ((cCenter.x > pt1.x) && (cCenter.x<pt2.x) && (cCenter.y>pt1.y) && (cCenter.y < pt2.y))
				{
					//////////오토바이 영역 픽셀 수 구하기//////////
					int cycleFix = 0;
					for (int i = 0; i < autoCycle.rows; i++)
						for (int j = 0; j < autoCycle.cols; j++)
							cycleFix += autoCycle.at<uchar>(i, j) / 255;

					//////////오토바이 영역 픽셀 수 구하기//////////
					int colFix = autoCycle.cols; //열 픽셀수
					int rowFix = autoCycle.rows; //행 픽셀수
					int allFix = colFix * rowFix; //전체 픽셀 수
					int colAvg = cycleFix / colFix;//열평균
					int rowAvg = cycleFix / rowFix;//행평균

					/////////열 가운데 영역 평균 구하기//////////
					int cFirst = colFix / 10 * 3;
					int cSecond = colFix / 10 * 7;
					int cCenterFix = 0;

					for (int i = 0; i < rowFix; i++)
						for (int j = cFirst; j <= cSecond; j++)
							cCenterFix += autoCycle.at<uchar>(i, j) / 255;

					int cCenterAvg = cCenterFix / (cSecond - cFirst);
					/////////열 가운데 영역 평균 구하기//////////

					/////////행 팔 영역 평균 구하기//////////
					int rFirst = rowFix / 7 * 2;
					int rSecond = rowFix / 7 * 3;
					int rArmFix = 0;

					for (int i = 0; i < rowFix; i++)
						for (int j = rFirst; j <= rSecond; j++)
							rArmFix += autoCycle.at<uchar>(i, j) / 255;

					int rArmAvg = rArmFix / (rSecond - rFirst);
					/////////행 팔 영역 평균 구하기//////////

					if (cCenterAvg > colAvg&&rArmAvg > rowAvg)
					{
						IplImage *correctCycle = &IplImage(frame);

						cvSetImageROI(correctCycle, label.m_recBlobs[i]);
						Mat corCycle = cvarrToMat(correctCycle);

						imwrite("correctCycle.png", corCycle);

						//////헬멧/ 번호판 영역 검출
						IplImage *mcPicture = cvLoadImage("correctCycle.png");
						Mat motorCycle = cvarrToMat(mcPicture);

						//헬멧 영역 검출
						Rect helmetRect(0, 0, mcPicture->width, mcPicture->height / 7 * 2);
						Mat helmetArea = motorCycle(helmetRect);

						//true면 번호판 검출
						if (helmetDetection(helmetArea))
						{
							Rect licensePlate(0, mcPicture->height / 7 * 5, mcPicture->width, mcPicture->height / 7 * 2);
							Mat licenseArea = motorCycle(licensePlate);
							imshow("licensePlateArea", licenseArea);

							//LicensePlate LicenseDetcet;
							//LicenseDetcet.isLicensePlateDetection(licenseArea);
						}

						waitKey(0);
					}
				}
			}
		}

		frameNum++;

		showVideo("grayImage", grayImage);
		showVideo("diffImage", diffImage);

		int ckey = waitKey(delay);
		if (ckey == 27) break;
	}
}
