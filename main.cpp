#include "main.h"

using namespace std;
using namespace cv;


int main() 
{
	string file = "Video\\t_b_1.mp4"; // 영상 file

	Motorcycle detection(file);       // 오토바이 검출 객체 파일입력

	detection.isMHLDetection(HELMET | MOTORCYCLE); //오토바이 운저자의 헬멧 미착용 검출 실행

	waitKey(0);

	return 0;
}
