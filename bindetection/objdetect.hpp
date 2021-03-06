#ifndef INC_OBJDETECT_HPP__
#define INC_OBJDETECT_HPP__

#include <iostream>
#include <sys/stat.h>
#include <opencv2/core/core.hpp>
#include <opencv2/gpu/gpu.hpp>

#include <vector>

// Base class for detector. Doesn't really do much - all of the heavy lifting is
// in the derived classes
class ObjDetect
{
   public :
      ObjDetect() : init_(false) {} //pass in value of false to cascadeLoaded
      virtual ~ObjDetect() {}       //empty destructor
      virtual void Detect(const cv::Mat &frame, std::vector<cv::Rect> &imageRects) = 0; //pure virtual function, must be defined by CPU and GPU detect
      virtual void Detect(const cv::gpu::GpuMat &frameGPUInput, std::vector<cv::Rect> &imageRects) 
      {
	 imageRects.clear();
      }
      bool initialized(void)
      {
	 return init_;
      }

   protected:
      bool init_;
};

// CPU version of cascade classifier
class CPU_CascadeDetect : public ObjDetect
{
   public :
      CPU_CascadeDetect(const char *cascadeName) : ObjDetect() // call default constructor of base class
      {
	 struct stat statbuf;
	 if (stat(cascadeName, &statbuf) != 0)
	 {
	    std::cerr << "Can not open classifier input " << cascadeName << std::endl;
	    std::cerr << "Try to point to a different one with --classifierBase= ?" << std::endl;
	    return;
	 }

	 init_ = classifier_.load(cascadeName);
      }
      ~CPU_CascadeDetect(void)
      {
      }
      void Detect(const cv::Mat &frame, std::vector<cv::Rect> &imageRects); //defined elsewhere

   private :
      cv::CascadeClassifier classifier_;
};

// CPU version of cascade classifier. Pretty much the same interface 
// as the CPU version, but with an added method to handle data 
// which is already moved to a GpuMat
class GPU_CascadeDetect : public ObjDetect
{
   public :
      GPU_CascadeDetect(const char *cascadeName) : ObjDetect()
      {
	 struct stat statbuf;
	 if (stat(cascadeName, &statbuf) != 0)
	 {
	    std::cerr << "Can not open classifier input " << cascadeName << std::endl;
	    std::cerr << "Try to point to a different one with --classifierBase= ?" << std::endl;
	    return;
	 }
	 init_ = classifier_.load(cascadeName);
      }
      ~GPU_CascadeDetect(void)
      {
	 classifier_.release();
      }
      void Detect (const cv::Mat          &frame,        std::vector<cv::Rect> &imageRects);
      void Detect (const cv::gpu::GpuMat &frameGPUInput, std::vector<cv::Rect> &imageRects);

   private :
      cv::gpu::CascadeClassifier_GPU classifier_;

      // Declare GPU Mat elements once here instead of every single
      // call to the functions which use them
      cv::gpu::GpuMat frameEq;
      cv::gpu::GpuMat frameGray;
      cv::gpu::GpuMat detectResultsGPU;
      cv::gpu::GpuMat uploadFrame;
};

extern int scale;
extern int neighbors;
extern int minDetectSize;
extern int maxDetectSize;

#endif
