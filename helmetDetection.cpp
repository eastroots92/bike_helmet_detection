#include "helmetDetection.h"


//헬멧 영역, 배경이미지, 헬멧 rect를 인자로 받는 생성자
Helmet::Helmet(Mat helmetArea, Mat bkgImage, Rect helmetRoI)
{
	this->helmetArea = helmetArea.clone();
	this->bkgImage = bkgImage.clone();
	this->helmetRoi = helmetRoi;
	cvtColor(helmetArea, gray, CV_RGB2GRAY);
}

/*
안전모 미착용 유무 판단 모듈
1차 판단 : adaboost를 이용한 분류를 통한 헬멧 유무 판단
2차 판단 : 영역의 픽셀 수가 50%이상의 크기라면 헬멧의 크기라고 판단
*/
int Helmet::isHelmetDetect()
{
	if (!helmet.load(CASCADENAME))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return 0;
	}

	int gr_thr = 4;
	double scale_step = 1.1;

	Size min_obj_sz(5, 5);
	Size max_obj_sz(100, 100);

	// run
	vector<Rect> found;
	helmet.detectMultiScale(gray, found, scale_step, gr_thr, 0, min_obj_sz, max_obj_sz); //분류기를 통한 분류 시작

	bool isHelmet;

	if ((int)found.size() == 0) // 착용자라면 실패
		return false;
	else                        //미착용 의심자 2차 판별
	{
		Rect hmRect(helmetRoi.x, helmetRoi.y, helmetRoi.width, helmetRoi.height); //헬멧영역 저장

		Mat helmetBkImg = bkgImage(hmRect);           //배경이미지에서 헬멧 영역 추출

		Mat hmDiffrent = gray.clone();                //헬멧 영역 이미지 복사

		absdiff(hmDiffrent, helmetBkImg, hmDiffrent); //헬멧 영역 이미지 - 헬멧 영역 배경이미지 차 이미지 저장

		threshold(hmDiffrent, hmDiffrent, 10, 255, THRESH_BINARY);  //임계값 10 이진화 

		Mat element4(4, 4, CV_8U, Scalar(1));                       //열기 연산 4x4 마스크
		Mat element3(3, 3, CV_8U, Scalar(1));                       //닫기 연산 3x3 마스크

		morphologyEx(hmDiffrent, hmDiffrent, MORPH_OPEN, element4); //잡티제거
		morphologyEx(hmDiffrent, hmDiffrent, MORPH_CLOSE, element3);//빈 영역 채우기

		//imshow("차영상", hmDiffrent);

		int helmetRatio = 0;

		destroyWindow("HelmetDetection");

		for (int i = 0; i < (int)found.size(); i++)
		{
			found[i].x = found[i].x - 5;
			found[i].y = found[i].y - 5;
			found[i].width = found[i].width + 5;
			found[i].height = found[i].height + 5;

			rectangle(helmetArea, found[i], Scalar(0, 255, 0), 2);

			int helmetFix = 0;

			for (int j = found[i].y; j < found[i].y + found[i].width; j++)      //헬멧 픽셀수 구하기
				for (int k = found[i].x; k < found[i].x + found[i].height; k++)
					helmetFix += hmDiffrent.at<uchar>(j, k) / 255;

			helmetRatio = (helmetFix * 100) / (found[i].width*found[i].height);

			imshow("HelmetDetection", helmetArea);

			if (helmetRatio >= 50) //면적이 50% 이상이라면 return helmet
				return HELMET;
			else                   //미 착용자라면 return false
				return false;
		}
	}
}

/*
안전모 미착용 유무 판단 모듈
1차 판단 : adaboost를 이용한 분류를 통한 헬멧 유무 판단
2차 판단 : 영역의 픽셀 수가 50%이상의 크기라면 헬멧의 크기라고 판단
*/
int Helmet::isHelmetDetect(Mat helmetArea, Mat bkgImage, Rect helmetRoi)
{
	cvtColor(helmetArea, gray, CV_RGB2GRAY);

	if (!helmet.load(CASCADENAME))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return 0;
	}

	int gr_thr = 4;
	double scale_step = 1.1;

	Size min_obj_sz(5, 5);
	Size max_obj_sz(100, 100);

	// run
	vector<Rect> found;
	//분류기를 통한 분류 시작
	helmet.detectMultiScale(gray, found, scale_step, gr_thr, 0, min_obj_sz, max_obj_sz); 

	bool isHelmet;

	if ((int)found.size() == 0) // 착용자라면 실패
		return false;
	else                        //미착용 의심자 2차 판별
	{
		Rect hmRect(helmetRoi.x, helmetRoi.y, helmetRoi.width, helmetRoi.height); //헬멧영역 저장

		Mat helmetBkImg = bkgImage(hmRect);           //배경이미지에서 헬멧 영역 추출

		Mat hmDiffrent = gray.clone();                //헬멧 영역 이미지 복사

		//헬멧 영역 이미지 - 헬멧 영역 배경이미지 차 이미지 저장
		absdiff(hmDiffrent, helmetBkImg, hmDiffrent); 

		threshold(hmDiffrent, hmDiffrent, 10, 255, THRESH_BINARY);  //임계값 10 이진화 

		Mat element4(4, 4, CV_8U, Scalar(1));                       //열기 연산 4x4 마스크
		Mat element3(3, 3, CV_8U, Scalar(1));                       //닫기 연산 3x3 마스크

		morphologyEx(hmDiffrent, hmDiffrent, MORPH_OPEN, element4); //잡티제거
		morphologyEx(hmDiffrent, hmDiffrent, MORPH_CLOSE, element3);//빈 영역 채우기

		//imshow("차영상", hmDiffrent);

		int helmetRatio = 0;

		destroyWindow("HelmetDetection");

		for (int i = 0; i < (int)found.size(); i++) 
		{
			found[i].x = found[i].x - 5;
			found[i].y = found[i].y - 5;
			found[i].width = found[i].width + 5;
			found[i].height = found[i].height + 5;
			
			rectangle(helmetArea, found[i], Scalar(0, 255, 0), 2);

			int helmetFix = 0;

			for (int j = found[i].y; j < found[i].y + found[i].width; j++) //헬멧 픽셀수 구하기
				for (int k = found[i].x; k < found[i].x + found[i].height; k++)
					helmetFix += hmDiffrent.at<uchar>(j, k) / 255;

			helmetRatio = (helmetFix * 100) / (found[i].width*found[i].height);

			if (helmetRatio >= 50) //면적이 50% 이상이라면 return helmet
				return HELMET;
			else                   //미 착용자라면 return false
				return false;

			imshow("HelmetDetection", helmetArea);
		}
	}
}