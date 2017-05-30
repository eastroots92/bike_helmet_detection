#pragma once

#include "opencv2\opencv.hpp"
#include <iostream>
#include <ctime>
#include "blob.h"

using namespace std;
using namespace cv;

#define LICENSEPLATE  8 //번호판 검출 성공시 리턴값

class LicensePlate
{
private:
	CBlobLabeling label;
	Mat bin;
	Mat imgRoi;
	string licenseNum;
	string type;
	stringstream f_name;
public:
	LicensePlate() { licenseNum = ""; type = ".jpg"; } //타입 및 licenseNum 초기화하는 매개변수가 없는 생성자
	int isLicensePlateDetection(Mat frame);            //입력 받은 번호판 영역의 번호판 검출하는 모듈
	void qSort(Mat data[], int numX[], int l, int r);  //x좌표를 기준으로 mat을 퀵정렬하는 모듈
	string findNumber(Mat number);                     //가장 비슷한 숫자를 텟플릿 매칭의 상관계수의 최대값 으로 찾는 모듈
public:
	double getRotationAngle(const IplImage* src);      //가장 긴 선의 회전각을 구하는 모듈
	void rotateImage(const IplImage* src, IplImage* dst, double degree); //회전각의 따라 이미지를 회전시키는 모듈
public:
	CBlobLabeling getLabel() { return label; }
	Mat getBin()             { return  bin; }
	Mat getImgRoi()          { return imgRoi; }
	string getNumber()       { return licenseNum; }
	string get_name()        { return f_name.str();}
};