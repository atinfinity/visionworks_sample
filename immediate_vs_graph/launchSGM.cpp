#include "launchSGM.hpp"
#include "vx_macro.hpp"
#include <NVX/nvx.h>
#include <opencv2/core.hpp>

double immediateSGM(const vx_context& context, const SGMParam& param, 
	const vx_image& v_left, const vx_image& v_right, vx_image& v_disp, const int loop_num)
{
	vx_uint32 width  = 0;
	vx_uint32 height = 0;
	VX_SAFE_CALL(vxQueryImage(v_left, VX_IMAGE_ATTRIBUTE_WIDTH, &width, sizeof(width)));
	VX_SAFE_CALL(vxQueryImage(v_left, VX_IMAGE_ATTRIBUTE_HEIGHT, &height, sizeof(height)));

	double time = 0.0;
	cv::TickMeter tm;
	for(int i = 0; i <= loop_num; i++)
	{
		tm.reset();
		tm.start();
		vx_image v_left_gray  = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
		vx_image v_right_gray = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
		VX_CHECK_REFERENCE(v_left_gray);
		VX_CHECK_REFERENCE(v_right_gray);

		VX_SAFE_CALL(vxuColorConvert(context, v_left, v_left_gray));
		VX_SAFE_CALL(vxuColorConvert(context, v_right, v_right_gray));

		VX_SAFE_CALL
		(
			nvxuSemiGlobalMatching(context, v_left_gray, v_right_gray, v_disp, 
				param.min_disparity, param.max_disparity, 
				param.P1, param.P2, param.sad, 
				param.ct_win_size, param.hc_win_size, 
				param.bt_clip_value, param.max_diff, 
				param.uniqueness_ratio, 
				param.scanlines_mask, param.flags)
		);

		VX_SAFE_CALL(vxReleaseImage(&v_left_gray));
		VX_SAFE_CALL(vxReleaseImage(&v_right_gray));
		tm.stop();
		time += (i > 0) ? tm.getTimeMilli() : 0.0;
	}
	time /= loop_num;

	return time;
}

double graphSGM(const vx_context& context, const SGMParam& param, 
	const vx_image& v_left, const vx_image& v_right, vx_image& v_disp, const int loop_num)
{
	// create graph
	vx_graph graph = vxCreateGraph(context);
	VX_CHECK_REFERENCE(graph);

	vx_uint32 width  = 0;
	vx_uint32 height = 0;
	VX_SAFE_CALL(vxQueryImage(v_left, VX_IMAGE_ATTRIBUTE_WIDTH, &width, sizeof(width)));
	VX_SAFE_CALL(vxQueryImage(v_left, VX_IMAGE_ATTRIBUTE_HEIGHT, &height, sizeof(height)));
	vx_image v_left_gray  = vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_U8);
	vx_image v_right_gray = vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_U8);
	VX_CHECK_REFERENCE(v_left_gray);
	VX_CHECK_REFERENCE(v_right_gray);

	vxColorConvertNode(graph, v_left, v_left_gray);
	vxColorConvertNode(graph, v_right, v_right_gray);
	nvxSemiGlobalMatchingNode(graph, v_left_gray, v_right_gray, v_disp, 
		param.min_disparity, param.max_disparity, 
		param.P1, param.P2, param.sad, 
		param.ct_win_size, param.hc_win_size, 
		param.bt_clip_value, param.max_diff, 
		param.uniqueness_ratio, 
		param.scanlines_mask, param.flags);

	//const char* option = "-O3";
	//VX_SAFE_CALL(vxSetGraphAttribute(graph, NVX_GRAPH_VERIFY_OPTIONS, option, strlen(option)));

	// verify graph
	VX_SAFE_CALL(vxVerifyGraph(graph));

	double time = 0.0;
	cv::TickMeter tm;
	for(int i = 0; i <= loop_num; i++)
	{
		tm.reset();
		tm.start();
		VX_SAFE_CALL(vxProcessGraph(graph));
		tm.stop();
		time += (i > 0) ? tm.getTimeMilli() : 0.0;
	}
	time /= loop_num;

	// release graph
	VX_SAFE_CALL(vxReleaseGraph(&graph));

	return time;
}
