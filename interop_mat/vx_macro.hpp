#pragma once

#include <VX/vx.h>
#include <sstream>

#define VX_THROW_EXCEPTION(msg)		\
	do { \
		std::ostringstream ostr_; \
		ostr_ << msg; \
		throw std::runtime_error(ostr_.str()); \
	} while(0)

#define VX_SAFE_CALL(vxOp) \
	do \
	{ \
		vx_status status = (vxOp); \
		if (status != VX_SUCCESS) \
		{ \
			VX_THROW_EXCEPTION(#vxOp << " failure [status = " << status << "]" << " in file " << __FILE__ << " line " << __LINE__); \
		} \
	} while (0)

#define VX_ASSERT(cond) \
	do \
	{ \
		if (!(cond)) \
		{ \
			VX_THROW_EXCEPTION(#cond << " failure in file " << __FILE__ << " line " << __LINE__); \
		} \
	} while (0)

#define VX_CHECK_REFERENCE(ref) \
	VX_ASSERT(ref != 0 && vxGetStatus((vx_reference)ref) == VX_SUCCESS)
