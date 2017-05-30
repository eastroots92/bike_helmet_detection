#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2\video\background_segm.hpp>
#include "bkgImage.h"
#include "blob.h"
#include "ShowVideo.h"
#include "helmetDetection.h"
#include "LicensePlate.h"
#include "mriaDB.h"

//MHL 검출 옵션
#define BKGIMG        1 //배경이미지
#define MOTORCYCLE    2 //오토바이
#define HELMET        4 //안전모
#define LICENSEPLATE  8 //번호판
#define ALL          15 //배경이미지, 오토바이, 안전모, 번호판 모두

using namespace std;
using namespace cv;

class Motorcycle
{
private:
	VideoCapture capture; //입력 영상
	string file;          //입력 파일 이름
	stringstream bkg_name;//배경 파일 이름
	Mat frame;            //영상의 프래임 저장
	Mat bkgImage;         //로드된 배경이미지
	Mat grayImage;        //흑백처리 영상
	CBlobLabeling label;  //레이블링 결과 저장
	BkgImage bkgImg;      //배경 이미지 추출을 위한 객체
	stringstream f_name;  //검출 시 저장할 파일의 이름
public:
	Motorcycle() {}          //매개변수 없는 생성자
	Motorcycle(string file); //파일 이름을 매겨변수 받는 생성자

public:
	void isMHLDetection();                 //오토바이 헬멧 번호판 종합 검출 함수
	void isMHLDetection(int option);       //오토바이 헬멧 번호판 일부분 검출 함수
public:
	int  isMotorcycleDetetion(Mat motocycle, Mat diffImage); //오토바이 검출
public:
	Mat getBackPicture()    { return bkgImage;}  //배경 이미지 get 메소드
	Mat getGrayImage()      { return grayImage; }//그레이 이미지 get 메소드
	string getFile()        { return file; }     //파일 이름 get 메소드
	CBlobLabeling getLabel(){ return label; }    //레이블링 정보 get 메소드

	void setFile(string file) { this->file = file; } // 파일 이름 set 메소드
};
