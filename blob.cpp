#include "blob.h"

/// @brief CBlobLabeling 생성자
/// @remarks Labeling에 사용되는 각종 변수 초기화
CBlobLabeling::CBlobLabeling()
{
	m_nThreshold = 0;
	m_nBlobs = _DEF_MAX_BLOBS;
	m_Image = NULL;

	m_vPoint = NULL;

	m_recBlobs = NULL;
	m_intBlobs = NULL;
}

/// @brief CBlobLabeling 소멸자
/// @remarks Labeling에 사용된 각종 변수 해제
CBlobLabeling::~CBlobLabeling()
{
	if (m_Image != NULL)
		cvReleaseImage(&m_Image);

	if (m_recBlobs != NULL)
	{
		delete m_recBlobs;
		m_recBlobs = NULL;
	}

	if (m_intBlobs != NULL)
	{
		delete m_intBlobs;
		m_intBlobs = NULL;
	}
}

/// @brief Parameter 설정
/// @remarks Labeling에 사용되는 이미지와 영역 크기 정보를 설정
/// @param[in] image        Labeling에 사용할 이미지. 0 또는 255의 값을 가지는 1ch 이미지.
/// @param[in] nThreshold   Labeling 결과 한 Blob의 픽셀 수가 nThreshold 이하인 경우 제거
void CBlobLabeling::SetParam(IplImage* image, int nThreshold)
{
	if (m_recBlobs != NULL)
	{
		delete m_recBlobs;

		m_recBlobs = NULL;
		m_nBlobs = _DEF_MAX_BLOBS;
	}

	if (m_intBlobs != NULL)
	{
		delete m_intBlobs;

		m_intBlobs = NULL;
		m_nBlobs = _DEF_MAX_BLOBS;
	}

	if (m_Image != NULL)	cvReleaseImage(&m_Image);

	m_Image = cvCloneImage(image);
	m_nThreshold = nThreshold;
}

/// @brief Labeling 실행
void CBlobLabeling::DoLabeling()
{
	m_nBlobs = Labeling(m_Image, m_nThreshold);
}

/// @brief Labeling 실행
/// @param[in] image        Labeling에 사용할 이미지. 0 또는 255의 값을 가지는 1ch 이미지.
/// @param[in] nThreshold   Labeling 결과 한 Blob의 픽셀 수가 nThreshold 이하인 경우 제거
/// @return Label 개수
int CBlobLabeling::Labeling(IplImage* image, int nThreshold)
{
	if (image->nChannels != 1) 	return 0;

	int nNumber;

	int nWidth = image->width;
	int nHeight = image->height;

	unsigned char* tmpBuf = new unsigned char[nWidth * nHeight];

	int i, j;

	for (j = 0; j<nHeight; j++)
		for (i = 0; i<nWidth; i++)
			tmpBuf[j*nWidth + i] = (unsigned char)image->imageData[j*image->widthStep + i];

	// 레이블링을 위한 포인트 초기화
	InitvPoint(nWidth, nHeight);

	// 레이블링
	nNumber = _Labeling(tmpBuf, nWidth, nHeight, nThreshold);

	// 포인트 메모리 해제
	DeletevPoint();

	if (nNumber != _DEF_MAX_BLOBS)     m_recBlobs = new CvRect[nNumber];

	if (nNumber != _DEF_MAX_BLOBS)     m_intBlobs = new int[nNumber];

	if (nNumber != 0)	DetectLabelingRegion(nNumber, tmpBuf, nWidth, nHeight);

	for (j = 0; j<nHeight; j++)
		for (i = 0; i<nWidth; i++)
			image->imageData[j*image->widthStep + i] = tmpBuf[j*nWidth + i];

	delete tmpBuf;

	return nNumber;
}

/// @brief m_vPoint 초기화
void CBlobLabeling::InitvPoint(int nWidth, int nHeight)
{
	int nX, nY;

	m_vPoint = new Visited[nWidth * nHeight];

	for (nY = 0; nY < nHeight; nY++)
	{
		for (nX = 0; nX < nWidth; nX++)
		{
			m_vPoint[nY * nWidth + nX].bVisitedFlag = false;
			m_vPoint[nY * nWidth + nX].ptReturnPoint.x = nX;
			m_vPoint[nY * nWidth + nX].ptReturnPoint.y = nY;
		}
	}
}

/// @brief m_vPoint 해제
void CBlobLabeling::DeletevPoint()
{
	delete m_vPoint;
	m_vPoint = NULL;
}

/// @brief Labeling 동작 함수
/// @remarks Size가 nWidth이고 nHeight인 DataBuf에서 nThreshold보다 작은 영역을 제외한 나머지를 blob으로 획득
/// @param[in] DataBuf      Labeling에 사용할 이미지 데이터 버퍼. 0 또는 255의 값을 가짐.
/// @param[in] nWidth       데이터 버퍼의 가로 길이
/// @param[in] nHeight      데이터 버퍼의 가로 길이
/// @param[in] nThreshold   Labeling 결과 한 Blob의 픽셀 수가 nThreshold 이하인 경우 제거
/// @return Label 개수
int CBlobLabeling::_Labeling(unsigned char* DataBuf, int nWidth, int nHeight, int nThreshold)
{
	int num = 0;
	int nX, nY, k, l;
	int StartX, StartY, EndX, EndY;

	// Find connected components
	for (nY = 0; nY < nHeight; nY++)
	{
		for (nX = 0; nX < nWidth; nX++)
		{
			if (DataBuf[nY * nWidth + nX] == 255)    // Is this a new component?, 255 == Object
			{
				num++;

				DataBuf[nY * nWidth + nX] = num;

				StartX = nX, StartY = nY, EndX = nX, EndY = nY;

				__NRFIndNeighbor(DataBuf, nWidth, nHeight, nX, nY, &StartX, &StartY, &EndX, &EndY);

				if (__Area(DataBuf, StartX, StartY, EndX, EndY, nWidth, num) < nThreshold)
				{
					for (k = StartY; k <= EndY; k++)
					{
						for (l = StartX; l <= EndX; l++)
						{
							if (DataBuf[k * nWidth + l] == num)
								DataBuf[k * nWidth + l] = 0;
						}
					}
					--num;

					if (num > 250)
						return  0;
				}
			}
		}
	}

	return num;
}

/// @brief Labeling 결과의 Blob 정보를 구하는 함수
/// @param[in] nLabelNumber 해당 Blob의 Label
/// @param[in] DataBuf      Labeling Result
/// @param[in] nWidth       DataBuf 가로 크기
/// @param[in] nHeight      DataBuf 세로 크기
void CBlobLabeling::DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight)
{
	int nX, nY;
	int nLabelIndex;

	bool bFirstFlag[255] = { false, };

	for (nY = 0; nY < nHeight; nY++)
	{
		for (nX = 0; nX < nWidth; nX++)
		{
			nLabelIndex = DataBuf[nY * nWidth + nX];

			if (nLabelIndex != 0)	// Is this a new component?, 255 == Object
			{
				if (bFirstFlag[nLabelIndex] == false)
				{
					m_recBlobs[nLabelIndex - 1].x = nX;
					m_recBlobs[nLabelIndex - 1].y = nY;
					m_recBlobs[nLabelIndex - 1].width = 0;
					m_recBlobs[nLabelIndex - 1].height = 0;

					bFirstFlag[nLabelIndex] = true;
				}
				else
				{
					int left = m_recBlobs[nLabelIndex - 1].x;
					int right = left + m_recBlobs[nLabelIndex - 1].width;
					int top = m_recBlobs[nLabelIndex - 1].y;
					int bottom = top + m_recBlobs[nLabelIndex - 1].height;

					if (left >= nX)	left = nX;
					if (right <= nX)	right = nX + 1;
					if (top >= nY)	top = nY;
					if (bottom <= nY)	bottom = nY + 1;

					m_recBlobs[nLabelIndex - 1].x = left;
					m_recBlobs[nLabelIndex - 1].y = top;
					m_recBlobs[nLabelIndex - 1].width = right - left;
					m_recBlobs[nLabelIndex - 1].height = bottom - top;

					m_intBlobs[nLabelIndex - 1] = nLabelIndex;
				}
			}
		}
	}
}

/// @brief Blob Labeling을 실제 행하는 함수
/// @param[in] DataBuf   Labeling에 사용할 이미지 데이터 버퍼. 0 또는 255의 값을 가짐.
/// @param[in] nWidth   데이터 버퍼의 가로 길이
/// @param[in] nHeight  데이터 버퍼의 가로 길이
/// @param[in] nPosX    현재 탐색중인 X좌표
/// @param[in] nPosY    현재 탐색중인 Y좌표
/// @param[out] StartX  탐색 시작지점 X좌표
/// @param[out] StartY  탐색 시작지점 Y좌표
/// @param[out] EndX    탐색 종료지점 X좌표
/// @param[out] EndY    탐색 종료지점 Y좌표
/// @return 0
int CBlobLabeling::__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY)
{
	CvPoint CurrentPoint;

	CurrentPoint.x = nPosX;
	CurrentPoint.y = nPosY;

	m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].bVisitedFlag = true;
	m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.x = nPosX;
	m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.y = nPosY;

	while (1)
	{
		if ((CurrentPoint.x != 0) && (DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x - 1] == 255))   // -X 방향
		{
			if (m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x - 1].bVisitedFlag == false)
			{
				DataBuf[CurrentPoint.y  * nWidth + CurrentPoint.x - 1] = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x - 1].bVisitedFlag = true;
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x - 1].ptReturnPoint = CurrentPoint;
				CurrentPoint.x--;

				if (CurrentPoint.x <= 0)
					CurrentPoint.x = 0;

				if (*StartX >= CurrentPoint.x)
					*StartX = CurrentPoint.x;

				continue;
			}
		}

		if ((CurrentPoint.x != nWidth - 1) && (DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x + 1] == 255))   // -X 방향
		{
			if (m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x + 1].bVisitedFlag == false)
			{
				DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x + 1] = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x + 1].bVisitedFlag = true;
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x + 1].ptReturnPoint = CurrentPoint;
				CurrentPoint.x++;

				if (CurrentPoint.x >= nWidth - 1)
					CurrentPoint.x = nWidth - 1;

				if (*EndX <= CurrentPoint.x)
					*EndX = CurrentPoint.x;

				continue;
			}
		}

		if ((CurrentPoint.y != 0) && (DataBuf[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x] == 255))   // -X 방향
		{
			if (m_vPoint[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x].bVisitedFlag == false)
			{
				DataBuf[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x] = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x].bVisitedFlag = true;
				m_vPoint[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x].ptReturnPoint = CurrentPoint;
				CurrentPoint.y--;

				if (CurrentPoint.y <= 0)
					CurrentPoint.y = 0;

				if (*StartY >= CurrentPoint.y)
					*StartY = CurrentPoint.y;

				continue;
			}
		}

		if ((CurrentPoint.y != nHeight - 1) && (DataBuf[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x] == 255))   // -X 방향
		{
			if (m_vPoint[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x].bVisitedFlag == false)
			{
				DataBuf[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x] = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x].bVisitedFlag = true;
				m_vPoint[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x].ptReturnPoint = CurrentPoint;
				CurrentPoint.y++;

				if (CurrentPoint.y >= nHeight - 1)
					CurrentPoint.y = nHeight - 1;

				if (*EndY <= CurrentPoint.y)
					*EndY = CurrentPoint.y;

				continue;
			}
		}

		if ((CurrentPoint.x == m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.x)
			&& (CurrentPoint.y == m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.y))
		{
			break;
		}
		else
		{
			CurrentPoint = m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint;
		}
	}

	return 0;
}

/// @brief Blob 영역 중 지정된 Label을 가진 영역의 크기(픽셀 수)를 구하는 함수
/// @param[in] DataBuf   Labeling에 사용할 이미지 데이터 버퍼
/// @param[in] StartX   탐색 시작지점 X좌표
/// @param[in] StartY   탐색 시작지점 Y좌표
/// @param[in] EndX     탐색 종료지점 X좌표
/// @param[in] EndY     탐색 종료지점 Y좌표
/// @param[in] nWidth   데이터 버퍼의 가로 길이
/// @param[in] nLevel   지정된 Blob의 Label
/// @return 탐색영역 내에서 지정된 Label의 픽셀 수
int CBlobLabeling::__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel)
{
	int nArea = 0;
	int nX, nY;

	for (nY = StartY; nY < EndY; nY++)
		for (nX = StartX; nX < EndX; nX++)
			if (DataBuf[nY * nWidth + nX] == nLevel)
				++nArea;

	return nArea;
}

/// @brief 가로/세로 비율이 지정한 비율보다 작은 Blob 제거
/// @remarks Width/Hight 가 fRatio보다 작을 경우, 그것들을 버리고, 결과를 rect로 변경\n
/// Input으로 들어온 rect와 그것의 개수 nRecNumber는 수행후, 변경된 값 들어감
/// @param[in] fRatio 가로/세로 비율
void CBlobLabeling::BlobWidthHeightSmallRatioConstraint(float fRatio)
{
	m_nBlobs = _BlobWidthHeightSmallRatioConstraint(fRatio, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief 가로/세로 비율이 지정한 비율보다 작은 Blob 제거하는 실제 함수
/// @param[in] fRatio   가로/세로 비율
/// @param[in] rect     Blob 영역 정보
/// @param[in] blobs    Blob Index 정보
/// @param[in] nRecNumber Blob 개수
/// @return 정리된 Blob 개수
int CBlobLabeling::_BlobWidthHeightSmallRatioConstraint(float fRatio, int nRecNumber, CvRect* rect, int* blobs)
{
	if (nRecNumber == 0)	return 0;

	int nX;
	int ntempRec = 0;

	CvRect *temp = new CvRect[nRecNumber];
	int *labeled = new int[nRecNumber];

	for (nX = 0; nX < nRecNumber; nX++)
	{
		temp[nX] = rect[nX];
		labeled[nX] = blobs[nX];
	}

	for (nX = 0; nX < nRecNumber; nX++)
	{
		if ((float)rect[nX].height / rect[nX].width > fRatio)
		{
			rect[ntempRec] = temp[nX];
			blobs[ntempRec] = labeled[nX];

			ntempRec++;
		}
	}

	delete temp;
	delete labeled;

	return ntempRec;
}


/// @brief 이미지 외곽 부분으로부터 지정한 거리 안에 있는(이미지 외곽에 붙은) Blob을 제거
/// @param[in] marginX  가로 거리 Threshold
/// @param[in] marginY  세로 거리 Threshold
void CBlobLabeling::BlobEdgeConstraint(int marginX, int marginY)
{
	m_nBlobs = _BlobEdgeConstraint(marginX, marginY, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief 이미지 외곽 부분으로부터 지정한 거리 안에 있는 Blob을 제거하는 실제 함수
/// @param[in] marginX      가로 거리 Threshold
/// @param[in] marginY      세로 거리 Threshold
/// @param[in] nRecNumber   Blob 개수
/// @param[in] rect         Blob 영역 정보
/// @param[in] blobs        Blob Index 정보
/// @return 정리된 Blob 개수
int CBlobLabeling::_BlobEdgeConstraint(int marginX, int marginY, int nRecNumber, CvRect *rect, int *label)
{
	if (nRecNumber == 0)	return 0;

	int nX;
	int ntempRec = 0;

	CvRect* temp = new CvRect[nRecNumber];
	int* labeled = new int[nRecNumber];

	for (nX = 0; nX < nRecNumber; nX++)
	{
		temp[nX] = rect[nX];
		labeled[nX] = label[nX];
	}

	for (nX = 0; nX < nRecNumber; nX++)
	{
		int l = rect[nX].x;
		int t = rect[nX].y;
		int r = rect[nX].x + rect[nX].width;
		int b = rect[nX].y + rect[nX].height;

		if (l > marginX && r < m_Image->width - marginX
			&&  t > marginY && b < m_Image->height - marginY)
		{
			temp[ntempRec] = rect[nX];
			labeled[ntempRec] = label[nX];

			ntempRec++;
		}
	}

	for (nX = 0; nX < ntempRec; nX++)
	{
		rect[nX] = temp[nX];
		label[nX] = labeled[nX];
	}

	delete temp;
	delete labeled;

	return ntempRec;
}

/// @brief 이미지를 둘러 싼 영역 내에서 실제 객체가 차지하는 비율이 지정한 값 보다 적은 Blob을 제거
/// @param[in] ratio 객체가 차지하는 비율 하한치
void CBlobLabeling::BlobIncludeRatioConstraint(float ratio)
{
	m_nBlobs = _BlobIncludeRatioConstraint(ratio, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief 이미지를 둘러 싼 영역 내에서 실제 객체가 차지하는 비율이 지정한 값 보다 적은 Blob을 제거하는 실제 함수
/// @param[in] ratio        객체가 차지하는 비율 하한치
/// @param[in] nRecNumber   Blob 개수
/// @param[in] rect         Blob 영역 정보
/// @param[in] blobs        Blob Index 정보
/// @return 정리된 Blob 개수
int CBlobLabeling::_BlobIncludeRatioConstraint(float ratio, int nRecNumber, CvRect *rect, int *label)
{
	if (nRecNumber == 0)	return 0;

	int nX;
	int ntempRec = 0;

	CvRect* temp = new CvRect[nRecNumber];
	int* labeled = new int[nRecNumber];

	for (nX = 0; nX < nRecNumber; nX++)
	{
		temp[nX] = rect[nX];
		labeled[nX] = label[nX];
	}

	for (nX = 0; nX < nRecNumber; nX++)
	{
		int cnt = 0;

		for (int j = 0; j < rect[nX].height; j++)
			for (int i = 0; i < rect[nX].width; i++)
			{
				unsigned char val = (unsigned char)m_Image->imageData[(j + rect[nX].y) * m_Image->widthStep + (i + rect[nX].x)];

				if (val == label[nX])	cnt++;
			}

		float constraint = (float)cnt / (rect[nX].width * rect[nX].height);

		if (constraint > ratio)
		{
			temp[ntempRec] = rect[nX];
			labeled[ntempRec] = label[nX];

			ntempRec++;
		}
	}

	for (nX = 0; nX < ntempRec; nX++)
	{
		rect[nX] = temp[nX];
		label[nX] = labeled[nX];
	}

	delete temp;
	delete labeled;

	return ntempRec;
}

/// @brief 원하는 블롭의 이미지 정보만 가져온다
/// @remarks 원하는 블롭의 이미지만 복사하므로, 함수 실행 이전에 이미지변수는 초기화 되어야 함.
/// @param[out] dest    블롭 이미지
/// @param[in] nLabel   원하는 Blob의 Label
/// @param[in] nX       image offset x좌표
/// @param[in] nY       image offset y좌표
void CBlobLabeling::GetBlobImage(IplImage* dest, int nLabel, int nX, int nY)
{
	CvRect rect = m_recBlobs[nLabel];
	int nNum = m_intBlobs[nLabel];

	for (int j = 0; j < rect.height; j++)
		for (int i = 0; i < rect.width; i++)
		{
			unsigned char val = (unsigned char)m_Image->imageData[(j + rect.y) * m_Image->widthStep + (i + rect.x)];

			if (val == nNum)   dest->imageData[(nY + j) * dest->widthStep + (nX + i)] = (unsigned char)255;
		}
}