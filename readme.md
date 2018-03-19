# OpenCV 기반 이륜차 안전모 미착용 자동 단속 시스템

  본 프로젝트는 이륜차 운전자들의 안전모 착용을 높이는 연구에대해 설명하려고 한다. 그 방법으로 본 연구팀이
선택한 방법은 openCV를 기반으로해, 이륜차 운전자들의 안전모 미착용자를 자동 단속하는 시스템에대해 연구하고 서술하고자 한다. CCTV 영상을 토대로 OpenCV기반의 영상처리 프로그래밍과 사물인식 알고리즘을 기반으로 위 의 시스템을 각각 이륜차 인식 및 판단 알고리즘, 안전모 착용 여부 판단 알고리즘, 번호판 인식 알고리즘 세 부분으로 나누어 향후 이륜차 안전모 미착용자 자동 단속 시스템 외에 국내 상황에 맞게 다양한 상황에서 활용할 수
있게 고려하였다. 이를 통하여 경찰관이 이륜차 안전모 미착용자를 현장 단속하는 것보다 효율적이고 안전하게 단
속할 수 있고 안전모 착용률을 높이는 효과를 기대할 수 있다.

[결과물 보기](https://youtu.be/cGlrYTJhHMU)

[개발 구현 보기](https://youtu.be/TWuktPMLp4Q)



* OpenCV Based Motorcycle Helmet Automatic Detection System

it is going to be explained to raise the rate of wearing safety helmet for people who drive bikes by using openCV as well as crackdown people who do not wear the safety helmet. By using openCV based Image processing programming and Object recognition algorithm, each bikes would be recognized based on CCTV video. The algorithm would be divided into whether the driver is wearing the safety helmet and recognition of the bike number. In the near future, this system would be utilized in diverse situations that could fit into domestic situations. By utilizing it, police officers can crackdown the people far safer and more efficiently who do not wear the safety helmets without going there.



## 1.  개발

* 사용 언어 : C++
* Open Source : OpenCV 3.0, Adaboost
* View : QT5
* DB :  MariaDB
* 개발환경 : VS 2013



## 2. 관련 연구

연구에서는 OpenCV 기반의 영상처리 프로그램과 사물인식 프로그램을 기반으로 국내의 여건에 맞는 단속 시스템을 구현하였다. 실제 단속시스템을 테스트하기 쉽지 않은 점을 고려하여 VMS솔루션이 아닌 영상을 받아와 처리하는 방식으로설계하였다. 이륜차 인식, 안전모 착용 여부 판별,번호판 추출 세부분으로 나누어서 구현하였다.

### 2.1 OpenCV (Open Source Computer Vision)

### 2.2 Cascade of HOG

### 2.3 번호판 인식 (LPR : License Plate Recognition)



## 3. 촬영 케이스

주행 속도를 20km부터 60km 까지 20씩 차등하여 촬영, 안전모 착용 미착용 여부, 비니와 일반 모자를 착용한 케이스 별로 촬영, 이륜차와 일반 차량 뿐만 아니라 트랙터, 버스 등과 같은 차랑과 사람 자전거 주행 Case를 각각 나누어 테스트를 하였다.



## 4. 구현

### 4.1 영상촬영 (입력/가공)

#### 	4.1.1 차영상

#### 	4.1.2 레이블링

### 4.2 이륜차 판단

#### 	4.2.1 이륜차 비율 판단

#### 	4.2.2 투영벡터를 이용한 이륜차 판단

### 4.3 안전모 판단

#### 	4.3.1 머리영역 판단 및 안전모 착용유무 판단

​		- Adaboost를 이용한 기계학습

#### 	4.3.2 머리영역의 원형 비율을 통한 안전모 판단

### 4.4 번호판 판단

#### 	4.4.1 번호판 영역 찾기

#### 	4.4.2 세선화

#### 	4.4.3 템플릿 매칭



## 5. 참여 인원

* [이동근](https://github.com/eastroots92)
* [김지민](https://github.com/jmk0427)
* 이상민
* 정정수