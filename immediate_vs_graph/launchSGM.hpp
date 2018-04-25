#pragma once

#include <VX/vx.h>
#include "SGMParam.hpp"

double immediateSGM(const vx_context& context, const SGMParam& param, 
	const vx_image& v_left, const vx_image& v_right, vx_image& v_disp, const int loop_num);
double graphSGM(const vx_context& context, const SGMParam& param, 
	const vx_image& v_left, const vx_image& v_right, vx_image& v_disp, const int loop_num);

