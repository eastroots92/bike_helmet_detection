
#include "bkgImage.h"


/* 배경이미지 구할 영상 파일이름을 매개변수로 받는 BkgImage 생성자
   입력 file명에 path, 확장자 제거 후 name 저장
   file의 실제 파일명 부분 앞의 bkg_를 붙인 bkg_name.jpg 배경이미지의 이름을
   bkg_name에 저장
*/
BkgImage::BkgImage(string file)
{
	this->file = file;
	frameNum = 0;
	string name = file.substr(file.find_last_of("/\\") + 1);
	size_t const p(name.find_last_of('.'));
	name = name.substr(0, p);
	bkg_name << "bkg_" << name << ".jpg";
}

/* 배경이미지 구할 영상 파일이름, 배경이미지 이름을 매개변수로 받는 BkgImage 생성자
*/
BkgImage::BkgImage(string file,string b_name)
{
	this->file = file;
	frameNum = 0;
	bkg_name << b_name;
}

/* 영상 평균 이미지 추출 모듈

영상 오픈 후 가로 세로 사이즈와 속도를 저장 후 프레임 단위로 읽으며 합을 구하여
총 합을 영상의 총 프레임 수로 나눠 평균을 구한 후 bkg_입력파일.jpg의 이름으로 파일을 저장하는 모듈

capture : 입력 영상 / size : 입력 영상 가로세로 사이즈 / fps 영상 frame 속도 / delay 처리를 위한 딜레이
sumImage : 영상의 평균 이미지 위한 임시 저장소
*/
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

	//영상읽기에 성공했다면
	while (capture.isOpened()) {

		capture >> frame;

		if (frame.empty()) break; //영상을 모두 읽었다면 종료

		cvtColor(frame, grayImage, COLOR_BGR2GRAY);

		accumulate(grayImage, sumImage);

		frameNum++;

		int ckey = waitKey(delay); //딜레이
		if (ckey == 27) break;     // 27 = Esc
	}

	sumImage = sumImage / (float)frameNum; //총합에 프레임수로 나눠 평균 이미지 추출

	imwrite(bkg_name.str(), sumImage);     //평균 이미지 저장
}

/* 배경 이미지 이름을 입력 인자로 받는 영상 평균 이미지 추출 모듈

  영상 오픈 후 가로 세로 사이즈와 속도를 저장 후 프레임 단위로 읽으며 합을 구하여
  총 합을 영상의 총 프레임 수로 나눠 평균을 구한 후 bkg_입력파일.jpg의 이름으로 파일을 저장하는 모듈

  capture : 입력 영상 / size : 입력 영상 가로세로 사이즈 / fps 영상 frame 속도 / delay 처리를 위한 딜레이
  sumImage : 영상의 평균 이미지 위한 임시 저장소
*/
void BkgImage::isAcummulateBkgImage(string b_name)
{
	capture.open(file);

	if (!capture.isOpened()) {
		cout << "비디오 파일이 없음" << endl;
		return;
	}

	bkg_name << b_name;

	size = Size((int)capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT));

	fps = (int)(capture.get(CAP_PROP_FPS));

	delay = 100 / fps;

	Mat sumImage(size, CV_32F, Scalar::all(0));

	//영상읽기에 성공했다면 무한루프
	while (capture.isOpened()) {

		capture >> frame;

		if (frame.empty()) break; //영상을 모두 읽었다면 종료

		cvtColor(frame, grayImage, COLOR_BGR2GRAY);

		accumulate(grayImage, sumImage);

		frameNum++;
		int ckey = waitKey(delay); //딜레이
		if (ckey == 27) break;     // 27 = Esc
	}

	sumImage = sumImage / (float)frameNum; //총합에 프레임수로 나눠 평균 이미지 추출

	imwrite(bkg_name.str(), sumImage);     //평균 이미지 저장
}