#pragma once

#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\video\background_segm.hpp>


using namespace std;
using namespace cv;

class BkgImage {
private:
	VideoCapture capture; //평균 이미지를 구할 영상
	Size size;            //영상의 가로세로 사이즈 저장
	string file;          //영상 파일의 이름 저장
	int fps;              //영상 프레임 속도 저장
	int delay;            //영상의 딜레이 조절을 위한 속도 저장 
	int frameNum;         //프레임 단위로 읽기 위한 변수
	Mat frame;            //영상의 프레임 저장
	Mat backPicture;      //평균 배경이미지 저장
	Mat grayImage;        //흑백영상 임시 저장
	stringstream bkg_name;//배경 파일 이름
public:
	BkgImage(){ frameNum = 0;}               //매개변수가 없는 생성자
	BkgImage(string file);                   //영상 파일 이름을 매개변수로 받는 생성자 
	BkgImage(string file, string b_name);    //영상 파일 이름, 배경이미지 이름을 매개변수로 받는 생성자
	void isAcummulateBkgImage();             //평균 배경 이미지 구하는 모듈
	void isAcummulateBkgImage(string b_name);//배경이미지 이름을 매개변수로 받아 평균 배경이미지를 구하는 모듈
public:
	int    getFps()        { return fps; }
	int    getDelay()      { return delay; }
	int    getFrameNum()   { return frameNum;}
	Mat    getBackPicture(){ return backPicture; }
	Mat    getGrayImage()  { return grayImage; }
	Size   getSize()       { return size; }
	string getFile()       { return file; }

	void setFile(string file) { this->file = file; }
};

