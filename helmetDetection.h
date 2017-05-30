#pragma once 
#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define HELMET 4 //헬멧 미착용자 검출시 리턴값

#define CASCADENAME "cascade.xml" //cascade 분류기 정보 xml 파일

class Helmet{
private:
	Mat helmetArea;
	Mat gray;
	Mat bkgImage;
	Rect helmetRoi;
private:
	CascadeClassifier helmet;
public:
	Helmet(){ } //매개변수가 없는 생성자
	Helmet(Mat helmetArea, Mat bkgImage, Rect helmetRoI); //헬멧영역, 배경이미지, 영역정보를 인자로 받는 생성자
public:
	int isHelmetDetect();      //헬멧 착용 유무를 판단하는 모듈
	int isHelmetDetect(Mat helmetArea, Mat bkgImage, Rect helmetRoI); 
	//헬멧영역, 배경이미지, 영역정보를 인자로 받아 착용 유무를 판단하는 모듈
public:
	void setHelmetArea(Mat helmetArea){ this->helmetArea = helmetArea.clone(); }
	void setBkgImage(Mat bkgImage)    { this->bkgImage = bkgImage.clone(); }
	void setHemetRoi(Rect helmetRoi)  { this->helmetRoi = helmetRoi; }
};
