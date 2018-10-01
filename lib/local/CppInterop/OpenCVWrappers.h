///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017, Carnegie Mellon University and University of Cambridge,
// all rights reserved.
//
// ACADEMIC OR NON-PROFIT ORGANIZATION NONCOMMERCIAL RESEARCH USE ONLY
//
// BY USING OR DOWNLOADING THE SOFTWARE, YOU ARE AGREEING TO THE TERMS OF THIS LICENSE AGREEMENT.  
// IF YOU DO NOT AGREE WITH THESE TERMS, YOU MAY NOT USE OR DOWNLOAD THE SOFTWARE.
//
// License can be found in OpenFace-license.txt

//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite at least one of the following works:
//
//       OpenFace 2.0: Facial Behavior Analysis Toolkit
//       Tadas Baltru�aitis, Amir Zadeh, Yao Chong Lim, and Louis-Philippe Morency
//       in IEEE International Conference on Automatic Face and Gesture Recognition, 2018  
//
//       Convolutional experts constrained local model for facial landmark detection.
//       A. Zadeh, T. Baltru�aitis, and Louis-Philippe Morency,
//       in Computer Vision and Pattern Recognition Workshops, 2017.    
//
//       Rendering of Eyes for Eye-Shape Registration and Gaze Estimation
//       Erroll Wood, Tadas Baltru�aitis, Xucong Zhang, Yusuke Sugano, Peter Robinson, and Andreas Bulling 
//       in IEEE International. Conference on Computer Vision (ICCV),  2015 
//
//       Cross-dataset learning and person-specific normalisation for automatic Action Unit detection
//       Tadas Baltru�aitis, Marwa Mahmoud, and Peter Robinson 
//       in Facial Expression Recognition and Analysis Challenge, 
//       IEEE International Conference on Automatic Face and Gesture Recognition, 2015 
//
///////////////////////////////////////////////////////////////////////////////

// OpenCVWrappers.h

#pragma once

#pragma managed

#include <msclr\marshal_cppstd.h>

#pragma unmanaged

#include <cv.h>
#include <highgui.h>

#include <opencv2/videoio/videoio.hpp>  // Video write
#include <opencv2/videoio/videoio_c.h>  // Video write

#include <ImageManipulationHelpers.h>

#pragma managed

#pragma make_public(cv::Mat)

using namespace System::Windows;
using namespace System::Windows::Threading;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;

namespace OpenCVWrappers {

	public ref class RawImage : System::IDisposable
	{

	private:

		cv::Mat* mat;

		RawImage()
		{
			mat = new cv::Mat();
		}

	public:

		static int PixelFormatToType(PixelFormat fmt)
		{
			if (fmt == PixelFormats::Gray8)
				return CV_8UC1;
			else if (fmt == PixelFormats::Bgr24)
				return CV_8UC3;
			else if (fmt == PixelFormats::Bgra32)
				return CV_8UC4;
			else if (fmt == PixelFormats::Gray32Float)
				return CV_32FC1;
			else
				throw gcnew System::Exception("Unsupported pixel format");
		}

		static PixelFormat TypeToPixelFormat(int type)
		{
			switch (type) {
			case CV_8UC1:
				return PixelFormats::Gray8;
			case CV_8UC3:
				return PixelFormats::Bgr24;
			case CV_8UC4:
				return PixelFormats::Bgra32;
			case CV_32FC1:
				return PixelFormats::Gray32Float;
			default:
				throw gcnew System::Exception("Unsupported image type");
			}
		}

		RawImage(const cv::Mat& m)
		{
			mat = new cv::Mat(m.clone());
		}

		RawImage(RawImage^ img)
		{
			mat = new cv::Mat(img->Mat.clone());
		}

		RawImage(int width, int height, int type, System::IntPtr scan, System::UInt64 step)
		{
			mat = new cv::Mat(height, width, type, scan.ToPointer(), step);
		}

		void Mirror()
		{
			cv::flip(*mat, *mat, 1);
		}

		// Finalizer. Definitely called before Garbage Collection,
		// but not automatically called on explicit Dispose().
		// May be called multiple times.
		!RawImage()
		{
			if (mat)
			{
				delete mat;
				mat = NULL;
			}
		}

		// Destructor. Called on explicit Dispose() only.
		~RawImage()
		{
			this->!RawImage();
		}

		property int Width
		{
			int get()
			{
				return mat->cols;
			}
		}

		property int Height
		{
			int get()
			{
				return mat->rows;
			}
		}

		property int Stride
		{

			int get()
			{
				return (int) mat->step;
			}
		}

		property PixelFormat Format
		{
			PixelFormat get()
			{
				return TypeToPixelFormat(mat->type());
			}
		}

		property cv::Mat& Mat
		{
			cv::Mat& get()
			{
				return *mat;
			}
		}

		property bool IsEmpty
		{
			bool get()
			{
				return !mat || mat->empty();
			}
		}

		bool UpdateWriteableBitmap(WriteableBitmap^ bitmap)
		{
			if (bitmap == nullptr || bitmap->PixelWidth != Width || bitmap->PixelHeight != Height || bitmap->Format != Format)
				return false;
			else {
				if (mat->data == NULL) {
					cv::Mat zeros(bitmap->PixelHeight, bitmap->PixelWidth, PixelFormatToType(bitmap->Format), 0);
					bitmap->WritePixels(Int32Rect(0, 0, Width, Height), System::IntPtr(zeros.data), Stride * Height * (Format.BitsPerPixel / 8), Stride, 0, 0);
				}
				else {
					bitmap->WritePixels(Int32Rect(0, 0, Width, Height), System::IntPtr(mat->data), Stride * Height * (Format.BitsPerPixel / 8), Stride, 0, 0);
				}
				return true;
			}
		}

		WriteableBitmap^ CreateWriteableBitmap()
		{
			return gcnew WriteableBitmap(Width, Height, 72, 72, Format, nullptr);
		}

		RawImage^ GetGreyImage()
		{
			RawImage^ grey = gcnew RawImage();
			Utilities::ConvertToGrayscale_8bit(*mat, grey->Mat);
			return grey;
		}
	};
}
