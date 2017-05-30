
#include "licensePlate.h"

/*
  출처 : github
  가장 긴 선분의 기울어진 각을 구하는 모듈 
*/
double LicensePlate :: getRotationAngle(const IplImage* src)
{
	// Only 1-Channel
	if (src->nChannels != 1)
		return 0;

	// 직선이 잘 검출될 수 있도록 팽창
	cvDilate((IplImage*)src, (IplImage*)src);

	// 저장영역 생성
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* seqLines;

	// Image의 직선영역을 seq에 저장(rho, theta)
	seqLines = cvHoughLines2((IplImage*)src, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 30, 3); 

	// 직선들의 거리를 구해서 가장 긴 직선을 기준으로 이미지 0 or 90도로 회전
	double    longDistance = 0;    // 직선 중 가장 긴 길이
	int        longDistanceIndex = 0;    // 직선 중 가장 긴 길이 인덱스

	for (int i = 0; i < seqLines->total; i++)
	{
		CvPoint* lines = (CvPoint*)cvGetSeqElem(seqLines, i);
		double euclideanDistance;        // sequence에 저장된 line들의 Euclidean distance를 저장
		euclideanDistance = (lines[1].x - lines[0].x) * (lines[1].x - lines[0].x) + (lines[1].y - lines[0].y) * (lines[1].y - lines[0].y);
		euclideanDistance = sqrt(euclideanDistance);

		// 가장 긴 Euclidean distance를 저장 
		if (longDistance < euclideanDistance)
		{
			longDistanceIndex = i;
			longDistance = euclideanDistance;
		}
	}

	// 회전된 각도 계산
	CvPoint *lines = (CvPoint*)cvGetSeqElem(seqLines, longDistanceIndex);
	int         dx = lines[1].x - lines[0].x;
	int         dy = lines[1].y - lines[0].y;
	double     rad = atan2((double)dx, (double)dy);    // 회전된 각도(radian)
	double  degree = abs(rad * 180) / CV_PI;            // 회전된 각도(degree) 저장

	// 메모리 해제
	cvClearSeq(seqLines);
	cvReleaseMemStorage(&storage);

	return degree;
}

/*
  출처 : github
  기울어진 각을 이용하여 회전하는 모듈
*/
void LicensePlate::rotateImage(const IplImage* src, IplImage* dst, double degree)
{
	// Only 1-Channel
	if (src->nChannels != 1)
		return;

	CvPoint2D32f centralPoint = cvPoint2D32f(src->width / 2, src->height / 2);            // 회전 기준점 설정(이미지의 중심점)
	CvMat *rotationMatrix = cvCreateMat(2, 3, CV_32FC1);                        // 회전 기준 행렬

	// Rotation 기준 행렬 연산 및 저장(90도에서 기울어진 각도를 빼야 본래이미지(필요시 수정))
	cv2DRotationMatrix(centralPoint, 90 - degree, 1, rotationMatrix);

	// Image Rotation
	cvWarpAffine(src, dst, rotationMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

	// Memory 해제
	cvReleaseMat(&rotationMatrix);
}

/*
	번호판 x좌표기준 정렬을 위한 퀵 정렬 모듈
*/
void LicensePlate :: qSort(Mat data[], int numX[], int l, int r)
{
	int left = l;
	int right = r;
	int pivot = numX[(l + r) / 2];

	do{
		while (numX[left] < pivot) left++;
		while (numX[right] > pivot) right--;
		if (left <= right){
			int temp = numX[left];
			numX[left] = numX[right];
			numX[right] = temp;

			Mat mTemp = data[left].clone();
			data[left] = data[right].clone();
			data[right] = mTemp.clone();

			left++;
			right--;
		}
	} while (left <= right);

	if (l < right) qSort(data, numX, l, right);
	if (r > left) qSort(data, numX, left, r);
}

/*
	넘겨받은 숫자 이미지에 대한템플릿 매칭을 이용한 
	상관계수의 Max값을 이용하여 가장 비슷한 숫자를 찾아내는 모듈
*/
string LicensePlate ::findNumber(Mat number)
{
	Mat findNum = imread("ten.bmp", CV_8U);
	
	threshold(findNum, findNum, 100, 255, THRESH_BINARY);

	CBlobLabeling labelFindNum;
	
	labelFindNum.SetParam(&(IplImage)findNum, 300);
	
	labelFindNum.DoLabeling();
	
	Mat numRoi[10];
	int numX[10];

	for (int i = 0; i < labelFindNum.m_nBlobs; i++)
	{
		numRoi[i] = findNum(Rect(labelFindNum.m_recBlobs[i].x, labelFindNum.m_recBlobs[i].y, labelFindNum.m_recBlobs[i].width, labelFindNum.m_recBlobs[i].height));
		numX[i] = labelFindNum.m_recBlobs[i].x;	
		resize(numRoi[i], numRoi[i], Size(50, 90));
	}

	qSort(numRoi, numX, 0, 9);

	CvPoint left_top;
	double min, max;
	double temp=0;
	int j = 0;

	//상관계수를 구할 이미지
	IplImage *coeff;
	for (int i = 0; i < labelFindNum.m_nBlobs; i++)
	{
		coeff = cvCreateImage(cvSize((&IplImage(number))->width - (&IplImage(numRoi[i]))->width + 1, (&IplImage(number))->height - (&IplImage(numRoi[i]))->height + 1), IPL_DEPTH_32F, 1);
		// 상관계수를 구하여 coeff에 그려준다.
		cvMatchTemplate(&IplImage(number), &IplImage(numRoi[i]), coeff, CV_TM_CCOEFF_NORMED);
		// 상관계수가 최대값을 가지는 위치를 찾는다 
		cvMinMaxLoc(coeff, &min, &max, NULL, &left_top);

		if (temp < max)
		{
			temp = max;
			j = i;
		}
	}
	return to_string(j);
}

/*
  레이블링 후 가로/세로 비율이 1.5 비율보다 작은영역 제거 후
  재레이블링 후 세로/가로 비율이 1.2 비율보다 작은영역 제거 후
  레이블링 수가 4개 인 부분을 번호판 영역이라고 판단.
  템플릿 매칭시 사이즈가 중요하여 리사이즈
*/
int LicensePlate::isLicensePlateDetection(Mat frame)
{
	if (frame.empty()) return 0;

	resize(frame, frame, Size(800, 500)); //템플릿 매칭을 위한 리사이즈

	bin = frame.clone();

	cvtColor(bin, bin, CV_BGR2GRAY);

	Mat binaryBin = bin.clone();

	for (int t = 0; t < 2; t++)
	{
		if (t == 0)
			threshold(bin, binaryBin, 50, 255, CV_THRESH_BINARY_INV | THRESH_OTSU);
		else if (t == 1) //이진화로 검출에 실패하였다면 역이진화 
			threshold(bin, binaryBin, 50, 255, CV_THRESH_BINARY | THRESH_OTSU);

		CBlobLabeling label;

		label.SetParam(&(IplImage)binaryBin, 500);
		label.DoLabeling();

		////////// 영역 세로/가로 비율 지정////////////
		label.BlobWidthHeightSmallRatioConstraint(1.5, HORIZONTAL);
		label.BlobBigSizeConstraint(600, 400);

		Mat labelimg = frame.clone();

		////////////////관심영역지정//////////////
		for (int i = 0; i < label.m_nBlobs; i++)
		{
			Mat roi = labelimg(Rect(label.m_recBlobs[i].x, label.m_recBlobs[i].y, label.m_recBlobs[i].width, label.m_recBlobs[i].height));

			resize(roi, roi, Size(350, 200));

			Mat grayRoi = roi.clone();

			cvtColor(grayRoi, grayRoi, CV_BGR2GRAY);

			Mat binaryRoi = grayRoi.clone();

			threshold(grayRoi, binaryRoi, 50, 255, CV_THRESH_BINARY_INV | THRESH_OTSU);//이진화

			CBlobLabeling relabel;

			relabel.SetParam(&(IplImage)binaryRoi, 900);
			relabel.DoLabeling();

			////////// 영역 가로/세로 비율 지정////////////
			relabel.BlobWidthHeightSmallRatioConstraint(1.2, VERTICAL);
			if (relabel.m_nBlobs != 4) //4개가 아니라면 다음 영역 검사
				continue;
			
			/////////////////////////////////////////////////////////////////
			if (relabel.m_nBlobs == 4)
			{
				t = 3;
				Mat numRoi = grayRoi.clone();
				Mat rotate = binaryRoi.clone(); //회전값 구하기

				int angle = getRotationAngle(&(IplImage)rotate); //회전시키기

				rotateImage(&(IplImage)rotate, &(IplImage)rotate, angle);//보여주기임

				Mat num[4];
				int nX[4];

				for (int j = 0; j < relabel.m_nBlobs; j++)
				{
					//4개의 번호 추출
					for (int k = 0; k < relabel.m_nBlobs; k++){
						num[k] = numRoi(Rect(relabel.m_recBlobs[k].x, relabel.m_recBlobs[k].y, relabel.m_recBlobs[k].width, relabel.m_recBlobs[k].height));
						nX[k] = relabel.m_recBlobs[k].x;
					}

					//번호를 x기준으로 퀵정렬 실행
					qSort(num, nX, 0, 3);

					Mat number = num[j];

					rotateImage(&(IplImage)number, &(IplImage)number, angle); //각각 회전

					threshold(number, number, 50, 255, CV_THRESH_BINARY_INV | THRESH_OTSU);

					resize(number, number, Size(50, 90));

					licenseNum += findNumber(number);

					destroyWindow("1");

					rectangle(roi, relabel.m_recBlobs[j], Scalar(255), 2);
				}
				cout << licenseNum;

				/*time_t curr;
				struct tm *d;
				curr = time(NULL);
				d = localtime(&curr);

				f_name << d->tm_year + 1900 << "." << d->tm_mon + 1 << "." << d->tm_mday << "_" << d->tm_hour << "." << d->tm_min << "." << d->tm_sec << "_" << licenseNum << type;
				imwrite(f_name.str(), roi);*/
				imshow("번호판 검출1", roi);

				return LICENSEPLATE;
			}
		}
	}
	return 0;
}