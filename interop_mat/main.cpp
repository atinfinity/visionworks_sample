#include <VX/vx.h>
#include <VX/vxu.h>
#include <NVX/nvx_opencv_interop.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "vx_macro.hpp"

//#define DISPLAY_IMAGE

int main(int argc, char* argv[])
{
	// load image
	cv::Mat src = cv::imread("lena.jpg", cv::IMREAD_GRAYSCALE);
	if(src.empty())
	{
		std::cout << "Error: Could not load image file." << std::endl;
		return -1;
	}
	cv::Mat dst(src.size(), src.type(), cv::Scalar(0));

	// create context
	vx_context context = vxCreateContext();
	VX_CHECK_REFERENCE(context);

	// convert from Mat to vx_image
	vx_image v_src = nvx_cv::createVXImageFromCVMat(context, src);
	vx_image v_dst = nvx_cv::createVXImageFromCVMat(context, dst);
	VX_CHECK_REFERENCE(v_src);
	VX_CHECK_REFERENCE(v_dst);

	// launch OpenVX API
	VX_SAFE_CALL(vxuBox3x3(context, v_src, v_dst));

	// convert from Mat to vx_image
	nvx_cv::VXImageToCVMatMapper mapper(v_dst, 0, NULL, VX_READ_AND_WRITE, VX_MEMORY_TYPE_HOST);
	cv::Mat result = mapper.getMat();

#ifdef DISPLAY_IMAGE
	cv::imshow("interop_mat", result);
	cv::waitKey(0);
#endif // DISPLAY_IMAGE

	// release resource
	VX_SAFE_CALL(vxReleaseImage(&v_src));
	VX_SAFE_CALL(vxReleaseImage(&v_dst));
	VX_SAFE_CALL(vxReleaseContext(&context));

	return 0;
}
