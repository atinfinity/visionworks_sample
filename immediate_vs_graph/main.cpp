#include <VX/vx.h>
#include <VX/vxu.h>
#include <NVX/nvx.h>
#include <NVX/nvx_opencv_interop.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "SGMParam.hpp"
#include "launchSGM.hpp"
#include "vx_macro.hpp"

//#define DISPLAY_IMAGE

void setDefaultParam(SGMParam& param)
{
	// this parameter is derived from official sample code
	param.min_disparity    = 0;
	param.max_disparity    = 64;
	param.P1               = 10;
	param.P2               = 100;
	param.sad              = 5;
	param.bt_clip_value    = 31;
	param.max_diff         = 32000;
	param.uniqueness_ratio = 0;
	param.scanlines_mask   = 85;
	param.ct_win_size      = 0;
	param.hc_win_size      = 1;
	param.flags            = NVX_SGM_PYRAMIDAL_STEREO;
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << "<left_image> <right_image>" << std::endl;
		return -1;
	}

	cv::Mat left  = cv::imread(argv[1], cv::IMREAD_COLOR);
	cv::Mat right = cv::imread(argv[2], cv::IMREAD_COLOR);
	if(left.empty() || right.empty())
	{
		std::cerr << "Error: Could not load image file." << std::endl;
		return -1;
	}

	// crop image for the limitation of VisionWorks
	cv::Rect roi(0, 0, (left.cols / 4) * 4, left.rows);
	left  = left(roi);
	right = right(roi);

	// create context
	vx_context context = vxCreateContext();
	VX_CHECK_REFERENCE(context);

	// convert from Mat to vx_image
	vx_image v_left  = nvx_cv::createVXImageFromCVMat(context, left);
	vx_image v_right = nvx_cv::createVXImageFromCVMat(context, right);
	vx_uint32 width  = 0;
	vx_uint32 height = 0;
	VX_SAFE_CALL(vxQueryImage(v_left, VX_IMAGE_ATTRIBUTE_WIDTH, &width, sizeof(width)));
	VX_SAFE_CALL(vxQueryImage(v_left, VX_IMAGE_ATTRIBUTE_HEIGHT, &height, sizeof(height)));
	vx_image v_disp = vxCreateImage(context, width, height, VX_DF_IMAGE_S16);
	VX_CHECK_REFERENCE(v_left);
	VX_CHECK_REFERENCE(v_right);
	VX_CHECK_REFERENCE(v_disp);

	const int loop_num = 10;

	// launch OpenVX API
	SGMParam param;
	setDefaultParam(param);
	double immediate_time = immediateSGM(context, param, v_left, v_right, v_disp, loop_num);
	double graph_time     = graphSGM(context, param, v_left, v_right, v_disp, loop_num);
	std::cout << "SGM(immediate mode) time = " << immediate_time << " ms." << std::endl;
	std::cout << "SGM(graph mode) time = " << graph_time << " ms." << std::endl;

#ifdef DISPLAY_IMAGE
	// convert from vx_image to Mat
	nvx_cv::VXImageToCVMatMapper mapper(v_disp, 0, NULL, VX_READ_AND_WRITE, VX_MEMORY_TYPE_HOST);
	cv::Mat disp = mapper.getMat();

	cv::Mat disp8u, colorMap, result;
	double min, max;
	cv::minMaxLoc(disp, &min, &max);
	disp.convertTo(disp8u, CV_8UC1, 255.0 / (max - min), -255.0 * min / (max - min));
	cv::applyColorMap(disp8u, colorMap, cv::COLORMAP_JET);
	cv::addWeighted(left, 0.3, colorMap, 0.7, 0, result);
	cv::imshow("result", result);
	cv::waitKey(0);
#endif // DISPLAY_IMAGE

	// release resource
	VX_SAFE_CALL(vxReleaseImage(&v_left));
	VX_SAFE_CALL(vxReleaseImage(&v_right));
	VX_SAFE_CALL(vxReleaseImage(&v_disp));
	VX_SAFE_CALL(vxReleaseContext(&context));

	return 0;
}

