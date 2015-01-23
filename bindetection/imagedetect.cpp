#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "imagedetect.hpp"

int scale     = 3;
int neighbors = 2;
int minDetectSize   = 20;
int maxDetectSize   = 200 * 4;
//int r_min     = 65;
//int r_max     = 90;
//int b_min     = 100;
//int b_max     = 170;
int hist_divider = 1;

int H_MIN =  53;
int H_MAX =  97;
int S_MIN =  30;
int S_MAX = 185;
int V_MIN =  57;
int V_MAX = 184;


const double DETECT_ASPECT_RATIO = 1.0;

using namespace std;
using namespace cv;
// Take an input image. Threshold it so that pixels within
// the HSV range specified by [HSV]_[MIN,MAX] are set to non-zero
// and the rest of the image is set to zero. Apply a morph
// open to the resulting image
static void generateThreshold(const Mat &ImageIn, Mat &ImageOut)
{
   Mat ThresholdLocalImage;
   vector<Mat> SplitImage;
   Mat SplitImageLE;
   Mat SplitImageGE;

   cvtColor(ImageIn, ThresholdLocalImage, CV_BGR2HSV, 0);
   split(ThresholdLocalImage, SplitImage);
   int max[3] = {H_MAX, S_MAX, V_MAX};
   int min[3] = {H_MIN, S_MIN, V_MIN};
   for (size_t i = 0; i < SplitImage.size(); i++)
   {
      compare(SplitImage[i], min[i], SplitImageGE, cv::CMP_GE);
      compare(SplitImage[i], max[i], SplitImageLE, cv::CMP_LE);
      bitwise_and(SplitImageGE, SplitImageLE, SplitImage[i]);
   }
   bitwise_and(SplitImage[0], SplitImage[1], ImageOut);
   bitwise_and(SplitImage[2], ImageOut, ImageOut);

   Mat erodeElement (getStructuringElement( MORPH_RECT,Size(3,3)));
   //dilate with larger element to make sure object is nicely visible
   Mat dilateElement(getStructuringElement( MORPH_ELLIPSE,Size(11,11)));
   erode(ImageOut, ImageOut, erodeElement, Point(-1,-1), 2);
   dilate(ImageOut, ImageOut, dilateElement, Point(-1,-1), 2);
}

void thresholdImage(const Mat &frame,  Mat &outFrame, vector <Rect> &rects)
{
   // Threshold the image using supplied HSV value
   vector < vector <Point> > contours;
   rects.clear();

   generateThreshold(frame, outFrame);
   Mat tempFrame = outFrame.clone();

   findContours(tempFrame, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

   // Find contours in the thresholded image.  Grab bounding
   // rectangles around those. Ideally, if the HSV values are 
   // set correctly, these rectangles will be the only
   // places that green-enough objects are found
   for (size_t i = 0; i < contours.size(); i++)
   {
      Rect rect = boundingRect(contours[i]);
      // Scale up rectangle a bit so that the
      // bounds are larger than each green blob detected
      const double RECT_SCALE_UP = 0.33;
      rect.x -= rect.height / (1/(RECT_SCALE_UP/2));
      rect.y -= rect.width / (1/(RECT_SCALE_UP/2));
      rect.height *= (1.0 + RECT_SCALE_UP);
      rect.width *= (1.0 + RECT_SCALE_UP);
      // Save bounding rects which are large enough
      // to actually contain the smallest image 
      // searched for
      if ((rect.width > (minDetectSize * DETECT_ASPECT_RATIO))&&(rect.height > minDetectSize))
	 rects.push_back(rect);
   }
}

/** @function detectAndDisplay */
void cascadeDetect ( const Mat &frame, 
      CascadeClassifier &cascade, 
      vector<Rect> &imageRects )
{
  Mat frameGray;
  cvtColor( frame, frameGray, CV_BGR2GRAY );
  equalizeHist( frameGray, frameGray );

  //-- Detect faces
  cascade.detectMultiScale( frameGray, 
	imageRects, 
	1.05 + scale/100., 
	neighbors, 
	0|CV_HAAR_SCALE_IMAGE, 
	Size(minDetectSize * DETECT_ASPECT_RATIO, minDetectSize), 
	Size(maxDetectSize * DETECT_ASPECT_RATIO, maxDetectSize) );
}

void filterUsingThreshold (const vector<Rect> &detectRects,
                           const vector<Rect> &threshRects,
			   vector<Rect> filteredRects)
{
   filteredRects.clear();
   for( size_t i = 0; i < detectRects.size(); i++ )  
   {
      // Hightlight detected images which are fully contained in 
      // green contour bounding rectangles
      bool inRect = false;
      for (size_t j = 0 ; !inRect && (j < threshRects.size()); j++)
      {
	 Rect intersect = detectRects[i] & threshRects[j];
	 if (intersect == detectRects[i])
	    inRect = true;
      }
      if (inRect)
	 filteredRects.push_back(detectRects[i]);
   }
}
