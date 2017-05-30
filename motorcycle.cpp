#include "motorcycle.h"

/*
  입력 file 이름을 매개변수로 받는 생성자
  입력 file명에 path, 확장자 제거 후 name 저장
  file의 실제 파일명 부분 앞의 bkg_를 붙인 bkg_name.jpg 배경이미지의 이름을 
  bkg_name에 저장
  ji
*/
Motorcycle ::Motorcycle(string file)
{
	this->file = file;
	string name = file.substr(file.find_last_of("/\\") + 1);
	size_t const p(name.find_last_of('.'));
	name = name.substr(0, p);
	bkg_name << "bkg_" << name << ".jpg";
}

/* 
 Motorcycle Helmet licenseplate all detect
 전처리 : absdiff (배경이미지 제거) -> threshold(이진화) -> 열기연산(잡티제거) -> 닫기연산(빈공간 채우기)-> 레이블링
          레이블링 된 객체 세로/가로비 1.5 비율이하 제거 후 중심 점이 center 위치시 검출 시작
 오토바이 검출(isMotorcycleDetetion) 성공 -> 헬멧 미착용시(!isHelmetDetect(helmetArea, bkgImage, helmetRect)) 
 -> 번호판 검출 isLicensePlateDetection(licenseArea);
*/
void Motorcycle::isMHLDetection()
{
	capture.open(file);      //file 문자열 이름의 영상 오픈

	if (!capture.isOpened()) //영상 여는 것을 실패 했다면 종료
	{
		cout << "비디오 파일이 없음" << endl;
		return;
	}

	bkgImage = imread(bkg_name.str(), COLOR_BGR2GRAY); //평균 배경 이미지 read

	if (bkgImage.empty())                              //비어 있다면 평균 배경 구하기
	{
		bkgImg.setFile(file);

		bkgImg.isAcummulateBkgImage(bkg_name.str());

		bkgImage = imread(bkg_name.str(), COLOR_BGR2GRAY);
	}

	int fps = (int)(capture.get(CAP_PROP_FPS));//입력 영상의 속도
	int nTh = 35;                              //이진화 임계값
	int frameNum = 0;                          //프레임 번호 저장(몇 프레임 단위로 읽을지 계산에 필요)
	int delay = 500 / fps;                     //프레임 속도 조절을 위한 딜레이 저장

	Mat frame, diffImage;                      //프레임과 평균 배경 차 이미지 저장
	Mat element4(4, 4, CV_8U, Scalar(1));      //모폴로지 열기 마스크
	Mat element25(25, 25, CV_8U, Scalar(1));   //모폴로지 닫기 마스크

	//영상 처리
	while (true)
	{
		capture >> frame;

		if (frame.empty()) break;

		cvtColor(frame, grayImage, COLOR_BGR2GRAY);

		//검출 영역
		IplImage *rect = &IplImage(grayImage);

		int rWidth = rect->width;	//rect의 가로길이
		int rHeight = rect->height;	//rect의 세로길이

		Rect rt(rWidth * 1 / 6, rHeight * 1 / 2, rWidth * 2 / 3, rHeight * 1 / 50);
		CvPoint pt1 = cvPoint(rWidth * 1 / 6, rHeight * 1 / 2);
		CvPoint pt2 = cvPoint(rWidth * 5 / 6, rHeight * 26 / 50);
		//Rect rt(가로 기준점, 세로 기준점, 너비, 높이

		rectangle(grayImage, rt, Scalar(255, 0, 0), 2);            //검출 영역 표시

		absdiff(grayImage, bkgImage, diffImage);                   //	입력 프레임(grayImage) - 평균 배경 이미지(bkgImage) = 차 영상 (diffImage)

		threshold(diffImage, diffImage, nTh, 255, THRESH_BINARY);  //임계치 보다 높으면 255로 변환 이진화

		morphologyEx(diffImage, diffImage, MORPH_OPEN, element4);   //잡티 제거 4x4 마스트 모폴로지 열기 연산

		morphologyEx(diffImage, diffImage, MORPH_CLOSE, element25); //빈공간 채우기 25x25 마스트 모폴로지 닫기 연산

		IplImage *trans = &IplImage(diffImage);                     //블롭 레이블링을 위한 mat(c++) -> IplImage(c) 변환

		label.SetParam(trans, 8000);       //8000 이하의 크기 제거
		label.DoLabeling();                //레이블링 진행
		label.BlobWidthHeightSmallRatioConstraint(1.5, VERTICAL);   //가로 세로비 1.5 미만 제거

		IplImage *cycle = &IplImage(diffImage);

		for (int i = 0; i < label.m_nBlobs; i++)
		{
			label.m_recBlobs[i].y = label.m_recBlobs[i].y - 10;
			label.m_recBlobs[i].height = label.m_recBlobs[i].height + 10;
			/*rectangle(grayImage, label.m_recBlobs[i], Scalar(255), 2);*/
			cvSetImageROI(cycle, label.m_recBlobs[i]);
			
			label.BlobBigSizeConstraint(4, 6);

			int cWidth = label.m_recBlobs[i].width;
			int cHeight = label.m_recBlobs[i].height;

			Mat motoCycle = cvarrToMat(cycle);

			CvPoint cPt = cvPoint(label.m_recBlobs[i].x, label.m_recBlobs[i].y);

			CvPoint cCenter = cvPoint(cPt.x + (cWidth / 2), cPt.y + (cHeight / 2));

			if ((cCenter.x > pt1.x) && (cCenter.x < pt2.x) && (cCenter.y > pt1.y) && (cCenter.y < pt2.y))
			{
				int result = isMotorcycleDetetion(motoCycle, diffImage);

				if (result) //오토바이 검출 성공했다면
				{
					imwrite("correctCycle.png", motoCycle);
					//헬멧 영역 검출
					Rect helmetRect(label.m_recBlobs[i].x, label.m_recBlobs[i].y, label.m_recBlobs[i].width, label.m_recBlobs[i].height / 7 * 2);
	
					Mat helmetArea = frame(helmetRect);

					Helmet hdect;

					//true면 번호판 검출
					if (!hdect.isHelmetDetect(helmetArea, bkgImage, helmetRect))
					{
						destroyWindow("licensePlateArea");
						Rect licensePlate(label.m_recBlobs[i].x, label.m_recBlobs[i].y + label.m_recBlobs[i].height / 7 * 5, label.m_recBlobs[i].width,
													label.m_recBlobs[i].height / 7 * 2);
						Mat licenseArea = frame(licensePlate);
						imshow("licensePlateArea", licenseArea);

						time_t curr;
						struct tm *d;
						curr = time(NULL);
						d = localtime(&curr);

						LicensePlate licenseDetcet;

						if (licenseDetcet.isLicensePlateDetection(licenseArea))
						{
							f_name.str(std::string()); //남아있는 string 지우기
							f_name.clear();

							//frame 저장
							f_name << d->tm_year + 1900 << "." << d->tm_mon + 1 << "." << d->tm_mday << "_" << d->tm_hour << "." 
										<< d->tm_min << "." << d->tm_sec << "_" << licenseDetcet.getNumber() << ".jpg";
							Mat store = frame(label.m_recBlobs[i]);
							imwrite(f_name.str(), store);

							//DB 저장
							MHLdata db;
							db.isInsertQuery(db.isSelectQuery(), f_name.str(), file, "", licenseDetcet.getNumber()); 
						}
					}
				}
			}
			cvResetImageROI(cycle);
		}
		frameNum++;

		showVideo("grayImage", grayImage); //기본 흑백영상 객체 추출 결과 표시 출력
		showVideo("diffImage", diffImage); //차 영상 이진화 출력

		int ckey = waitKey(delay);         //키 입력 대기 시간
		if (ckey == 27) break;
	}
}

/* 
  Motorcycle Helmet licenseplate select detect
   전처리 : absdiff (배경이미지 제거) -> threshold(이진화) -> 열기연산(잡티제거) -> 닫기연산(빈공간 채우기)-> 레이블링
  레이블링 된 객체 세로/가로비 1.5 비율이하 제거 후 중심 점이 center 위치시 검출 시작
  선택되어진 옵션에 따라 원하는 부분 검출
  option : MOTORCYCLE  -> 오토바이 검출
           HELMET      -> 안전모 미착용자 검출
	       LICENSEPLATE-> 번호판 검출
*/
void Motorcycle::isMHLDetection(int option)
{
	capture.open(file);      //file 문자열 이름의 영상 오픈

	if (!capture.isOpened()) //영상 여는 것을 실패 했다면 종료
	{
		cout << "비디오 파일이 없음" << endl;
		return;
	}

	if (option & BKGIMG)
	{
		bkgImg.setFile(file);

		bkgImg.isAcummulateBkgImage(bkg_name.str());

		bkgImage = imread(bkg_name.str(), COLOR_BGR2GRAY);
	}

	bkgImage = imread(bkg_name.str(), COLOR_BGR2GRAY); //평균 배경 이미지 read

	if (bkgImage.empty())                              //비어 있다면 평균 배경 구하기
	{
		bkgImg.setFile(file);

		bkgImg.isAcummulateBkgImage(bkg_name.str());

		bkgImage = imread(bkg_name.str(), COLOR_BGR2GRAY);
	}

	int fps = (int)(capture.get(CAP_PROP_FPS));//입력 영상의 속도
	int nTh = 35;                              //이진화 임계값
	int frameNum = 0;                          //프레임 번호 저장(몇 프레임 단위로 읽을지 계산에 필요)
	int delay = 500 / fps;                     //프레임 속도 조절을 위한 딜레이 저장

	Mat frame, diffImage;                      //프레임과 평균 배경 차 이미지 저장
	Mat element4(4, 4, CV_8U, Scalar(1));      //모폴로지 열기 마스크
	Mat element25(25, 25, CV_8U, Scalar(1));   //모폴로지 닫기 마스크

	while (true)
	{
		capture >> frame;

		if (frame.empty()) break;

		cvtColor(frame, grayImage, COLOR_BGR2GRAY);

		//검출 영역
		IplImage *rect = &IplImage(grayImage);

		int rWidth = rect->width;	//rect의 가로길이
		int rHeight = rect->height;	//rect의 세로길이

		Rect rt(rWidth * 1 / 6, rHeight * 1 / 2, rWidth * 2 / 3, rHeight * 1 / 50);
		CvPoint pt1 = cvPoint(rWidth * 1 / 6, rHeight * 1 / 2);
		CvPoint pt2 = cvPoint(rWidth * 5 / 6, rHeight * 26 / 50);
		//Rect rt(가로 기준점, 세로 기준점, 너비, 높이

		rectangle(grayImage, rt, Scalar(255, 0, 0), 2); //검출 영역 표시

		absdiff(grayImage, bkgImage, diffImage);        //입력 프레임(grayImage) - 평균 배경 이미지(bkgImage) = 차 영상 (diffImage)

		threshold(diffImage, diffImage, nTh, 255, THRESH_BINARY);  //임계치 보다 높으면 255로 변환 이진화

		morphologyEx(diffImage, diffImage, MORPH_OPEN, element4);   //잡티 제거 5x5 마스트 모폴로지 열기 연산
		morphologyEx(diffImage, diffImage, MORPH_CLOSE, element25); //빈공간 채우기 50x50 마스트 모폴로지 닫기 연산

		IplImage *trans = &IplImage(diffImage);         //블롭 레이블링을 위한 mat(c++) -> IplImage(c) 변환

		label.SetParam(trans, 8000);                    //8000 이하의 크기 제거
		label.DoLabeling();                             //레이블링 진행

		if (option & MOTORCYCLE)
			label.BlobWidthHeightSmallRatioConstraint(1.5, VERTICAL); //가로 세로비 1.5 미만 제거

		IplImage *object = &IplImage(diffImage);

		for (int i = 0; i < label.m_nBlobs; i++)
		{
			label.m_recBlobs[i].y = label.m_recBlobs[i].y - 10;
			label.m_recBlobs[i].height = label.m_recBlobs[i].height + 10;
			/*rectangle(grayImage, label.m_recBlobs[i], Scalar(255), 2);*/
			cvSetImageROI(object, label.m_recBlobs[i]);

			label.BlobBigSizeConstraint(4, 6);

			int cWidth = label.m_recBlobs[i].width;
			int cHeight = label.m_recBlobs[i].height;

			Mat object_mat = cvarrToMat(object);
			Mat helmetArea;
			Mat licenseArea;

			CvPoint cPt = cvPoint(label.m_recBlobs[i].x, label.m_recBlobs[i].y);

			CvPoint cCenter = cvPoint(cPt.x + (cWidth / 2), cPt.y + (cHeight / 2));

			if ((cCenter.x > pt1.x) && (cCenter.x < pt2.x) && (cCenter.y > pt1.y) && (cCenter.y < pt2.y))
			{
				int result = 0;
				LicensePlate licenseDetcet;
				Helmet hdect;

				if (option & MOTORCYCLE)
					result += isMotorcycleDetetion(object_mat, diffImage);

				if (option & MOTORCYCLE)
				{
					Rect helmetRect(label.m_recBlobs[i].x, label.m_recBlobs[i].y, label.m_recBlobs[i].width,
											label.m_recBlobs[i].height / 7 * 2);

					helmetArea = frame(helmetRect);

					result += hdect.isHelmetDetect(helmetArea, bkgImage, helmetRect);
				}

				//true면 번호판 검출
				if (option & LICENSEPLATE)
				{
					Rect licensePlate(label.m_recBlobs[i].x, label.m_recBlobs[i].y + label.m_recBlobs[i].height / 7 * 5,
											label.m_recBlobs[i].width, label.m_recBlobs[i].height / 7 * 2);
					
					licenseArea = frame(licensePlate).clone();

					result += licenseDetcet.isLicensePlateDetection(licenseArea);
				}

				if ((result & ALL) == option)
				{
					time_t curr;
					struct tm *d;
					curr = time(NULL);
					d = localtime(&curr);

					f_name.str(std::string()); //남아있는 string 지우기
					f_name.clear();

					//frame 저장
					f_name << d->tm_year + 1900 << "." << d->tm_mon + 1 << "." << d->tm_mday << "_" << d->tm_hour << "."
						<< d->tm_min << "." << d->tm_sec << "_" << licenseDetcet.getNumber() << ".jpg";
					Mat store = frame(label.m_recBlobs[i]);
					imwrite(f_name.str(), store);

					//DB 저장
					MHLdata db;
					db.isInsertQuery(db.isSelectQuery(), f_name.str(), file, "", licenseDetcet.getNumber());
				}
			}
			cvResetImageROI(object);
		}
		frameNum++;

		showVideo("grayImage", grayImage); //기본 흑백영상 객체 추출 결과 표시 출력
		showVideo("diffImage", diffImage); //차 영상 이진화 출력

		int ckey = waitKey(delay);         //키 입력 대기 시간
		if (ckey == 27) break;
	}
}

/*
	투영 벡터의 가로 세로의 값의 일정 센터 부분의 평균을 구한 뒤
	전체 이미지의 평균 투영 벡터의 값보다 크다면 오토바이로 판별하는 모듈
	parameter -> motocycle :오토바이 의심 영역  diffImage : 배경 차 영상
*/
int Motorcycle::isMotorcycleDetetion(Mat motocycle,Mat diffImage)
{
	IplImage *rect = &IplImage(grayImage);

	IplImage *cycle = &IplImage(diffImage);

	//////////오토바이 영역 픽셀 수 구하기//////////
	int cycleFix = 0;
	for (int i = 0; i < motocycle.rows; i++)
		for (int j = 0; j < motocycle.cols; j++)
			cycleFix += motocycle.at<uchar>(i, j) / 255;

	//////////오토바이 영역 픽셀 수 구하기//////////
	int colFix = motocycle.cols; //열 픽셀수
	int rowFix = motocycle.rows; //행 픽셀수
	int allFix = colFix * rowFix; //전체 픽셀 수
	int colAvg = cycleFix / colFix;//열평균
	int rowAvg = cycleFix / rowFix;//행평균

	/////////열 가운데 영역 평균 구하기//////////
	int cFirst = colFix / 10 * 3;
	int cSecond = colFix / 10 * 7;
	int cCenterFix = 0;

	for (int i = 0; i < rowFix; i++)
		for (int j = cFirst; j <= cSecond; j++)
			cCenterFix += motocycle.at<uchar>(i, j) / 255;

	int cCenterAvg = cCenterFix / (cSecond - cFirst);
	/////////열 가운데 영역 평균 구하기//////////

	/////////행 팔 영역 평균 구하기//////////
	int rFirst = rowFix / 7 * 2;
	int rSecond = rowFix / 7 * 3;
	int rArmFix = 0;

	for (int i = 0; i < rowFix; i++)
		for (int j = rFirst; j <= rSecond; j++)
			rArmFix += motocycle.at<uchar>(i, j) / 255;

	int rArmAvg = rArmFix / (rSecond - rFirst);
	/////////행 팔 영역 평균 구하기//////////

	if (cCenterAvg > colAvg&&rArmAvg > rowAvg) return MOTORCYCLE;

	else return false;
}


