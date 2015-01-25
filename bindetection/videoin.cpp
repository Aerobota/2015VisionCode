#include "videoin.hpp"

using namespace cv;
using namespace std;

VideoIn::VideoIn(const char *path)
{
   if (strstr(path, ".png") || strstr(path, ".jpg"))
   {
      _frame = imread(path);
      _video = false;
   }
   else
   {
      _cap = VideoCapture(path);
      _video = true;
   }
   _frameCounter = 0;
}
VideoIn::VideoIn(int stream)
{
   _cap = VideoCapture(stream);
   _video = true;
   _frameCounter = 0;
}

bool VideoIn::getNextFrame(bool pause, Mat &frame)
{
   if (!pause && _video)
   {
      _cap >> _frame;
      if( _frame.empty() )
	 return false;
      if (frame.cols > 800)
	 pyrDown(_frame, _frame);
      _frameCounter += 1;
   }
   frame = _frame.clone();

   return true;
}
int VideoIn::frameCounter(void)
{
   return _frameCounter;
}