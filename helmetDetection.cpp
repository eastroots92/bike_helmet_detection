#include "helmetDetection.h"


bool helmetDetection(Mat helmetArea)
{
	Mat gray;
	cvtColor(helmetArea, gray, CV_RGB2GRAY);
	string cascadeName = "cascade.xml";
	CascadeClassifier helmet;

	helmet.load(cascadeName);

	if (!helmet.load(cascadeName))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return 0;
	}

	int gr_thr = 4;
	double scale_step = 1.1;
	Size min_obj_sz(5, 5);
	Size max_obj_sz(100, 100);

	// run
	vector<Rect> found;
	helmet.detectMultiScale(gray, found, scale_step, gr_thr, 0, min_obj_sz, max_obj_sz);

	if ((int)found.size() == 0)
		return false;
	else
	{
		for (int i = 0; i < (int)found.size(); i++)
			rectangle(helmetArea, found[i], Scalar(0, 255, 0), 2);
		imshow("HelmetDetection", helmetArea);
		return true;
	}
}