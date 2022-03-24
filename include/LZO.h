#pragma once

#include <Always.h>

#include <LZOConfig.h>

int lzo1x_1_compress(const lzo_byte* in,
	lzo_uint  in_len,
	lzo_byte* out,
	lzo_uint* out_len,
	lzo_voidp wrkmem);

int lzo1x_decompress(const lzo_byte* in,
	lzo_uint  in_len,
	lzo_byte* out,
	lzo_uint* out_len,
	lzo_voidp);