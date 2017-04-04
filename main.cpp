#include "main.h"

using namespace std;
using namespace cv;


int main() 
{
	string file = "Video\\2.mp4";

	Motosycle detection(file);

	detection.isMotosycleDetection();

	return 0;
}
