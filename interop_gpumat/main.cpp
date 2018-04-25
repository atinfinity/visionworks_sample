#include <VX/vx.h>
#include <VX/vxu.h>
#include <NVX/nvx_opencv_interop.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
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
		std::cerr << "Error: Could not load image file." << std::endl;
		return -1;
	}
	cv::Mat dst(src.size(), src.type(), cv::Scalar(0));
	cv::cuda::GpuMat d_src(src);
	cv::cuda::GpuMat d_dst(dst);

	// create context
	vx_context context = vxCreateContext();
	VX_CHECK_REFERENCE(context);

	// convert from GpuMat to vx_image
	vx_image v_dsrc = nvx_cv::createVXImageFromCVGpuMat(context, d_src);
	vx_image v_ddst = nvx_cv::createVXImageFromCVGpuMat(context, d_dst);
	VX_CHECK_REFERENCE(v_dsrc);
	VX_CHECK_REFERENCE(v_ddst);

	// launch OpenVX API
	VX_SAFE_CALL(vxuBox3x3(context, v_dsrc, v_ddst));

	// convert from vx_image to GpuMat
	nvx_cv::VXImageToCVMatMapper mapper(v_ddst, 0, NULL, VX_READ_AND_WRITE, NVX_MEMORY_TYPE_CUDA);
	cv::cuda::GpuMat d_result = mapper.getGpuMat();

#ifdef DISPLAY_IMAGE
	cv::Mat result;
	d_result.download(result);
	cv::imshow("interop_gpumat", result);
	cv::waitKey(0);
#endif // DISPLAY_IMAGE

	// release resource
	VX_SAFE_CALL(vxReleaseImage(&v_dsrc));
	VX_SAFE_CALL(vxReleaseImage(&v_ddst));
	VX_SAFE_CALL(vxReleaseContext(&context));

	return 0;
}
