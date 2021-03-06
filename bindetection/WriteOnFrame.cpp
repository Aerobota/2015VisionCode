#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/opencv.hpp>
#include <time.h>
#include "WriteOnFrame.hpp"

using namespace std;
using namespace cv;

WriteOnFrame::WriteOnFrame(const Mat &setTo) {
	image = setTo.clone();
}

void WriteOnFrame::writeTime() { //write the time on the frame
	time_t rawTime;
	time(&rawTime);
	struct tm * localTime;
	localTime = localtime(&rawTime);
	char arrTime[100];
	strftime(arrTime, sizeof(arrTime), "%T %D", localTime);
	putText(image,string(arrTime), Point(0,20), FONT_HERSHEY_TRIPLEX, 0.75, Scalar(147,20,255), 1);
}

void WriteOnFrame::writeMatchNumTime(string matchNum, double matchTime) {
	string matchTimeString;
	if (matchNum != "No Match Number")
		matchNum = "Match Number:" + matchNum;
	if (matchTime == -1) 
		matchTimeString = "No Match Time";
	else {
		matchTimeString = "Match Time: ";
		stringstream s;
		s << matchTimeString;
		s << matchTime;
		matchTimeString = s.str();
 	}
	putText(image,matchNum,Point(0,40), FONT_HERSHEY_TRIPLEX, 0.75, Scalar(147,20,255), 1);
	putText(image,matchTimeString,Point(0,60), FONT_HERSHEY_TRIPLEX, 0.75, Scalar(147,20,255), 1);
}

void WriteOnFrame::write(VideoWriter &writeTo) {
	writeTo << image;
}
