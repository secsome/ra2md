#pragma once

#include <Always.h>

#include <LZO.h>

#define LZO1X_MEM_COMPRESS      ((lzo_uint) (16384L * sizeof(lzo_byte *)))
#define LZO1X_MEM_DECOMPRESS    (0)


/* fast decompression */
LZO_EXTERN(int)
lzo1x_decompress(const lzo_byte* src, lzo_uint  src_len,
    lzo_byte* dst, lzo_uint* dst_len,
    lzo_voidp wrkmem /* NOT USED */);

/* safe decompression with overrun testing */
LZO_EXTERN(int)
lzo1x_decompress_x(const lzo_byte* src, lzo_uint  src_len,
    lzo_byte* dst, lzo_uint* dst_len,
    lzo_voidp wrkmem /* NOT USED */);


/***********************************************************************
//
************************************************************************/

LZO_EXTERN(int)
lzo1x_1_compress(const lzo_byte* src, lzo_uint  src_len,
    lzo_byte* dst, lzo_uint* dst_len,
    lzo_voidp wrkmem);


/***********************************************************************
// better compression ratio at the cost of more memory and time
************************************************************************/

#define LZO1X_999_MEM_COMPRESS  ((lzo_uint) (14 * 16384L * sizeof(short)))

LZO_EXTERN(int)
lzo1x_999_compress(const lzo_byte* src, lzo_uint  src_len,
    lzo_byte* dst, lzo_uint* dst_len,
    lzo_voidp wrkmem);