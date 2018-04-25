#pragma once

#include <VX/vx.h>

struct SGMParam
{
	// disparity range
	vx_int32 min_disparity;
	vx_int32 max_disparity;

	// discontinuity penalties
	vx_int32 P1;
	vx_int32 P2;

	// SAD window size
	vx_int32 sad;

	// Census Transform window size
	vx_int32 ct_win_size;

	// Hamming cost window size
	vx_int32 hc_win_size;

	// BT-cost clip value
	vx_int32 bt_clip_value;

	// validation threshold
	vx_int32 max_diff; // cross-check
	vx_int32 uniqueness_ratio;

	vx_enum scanlines_mask;
	vx_enum flags;
};

