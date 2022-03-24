#include <LZO1X.h>

#ifndef NDEBUG
#define NDEBUG
#endif
#include <assert.h>

#if !defined(LZO1X) && !defined(LZO1Y)
#  define LZO1X
#endif

#if 1
#  define TEST_IP				1
#else
#  define TEST_IP				(ip < ip_end)
#endif


/***********************************************************************
// decompress a block of data.
************************************************************************/

int lzo1x_decompress(const lzo_byte* in, lzo_uint  in_len,
	lzo_byte* out, lzo_uint* out_len,
	lzo_voidp)
{
	lzo_byte* op;
	const lzo_byte* ip;
	lzo_uint t;
	const lzo_byte* m_pos;
	const lzo_byte* const ip_end = in + in_len;

	*out_len = 0;

	op = out;
	ip = in;

	if (*ip > 17)
	{
		t = *ip++ - 17;
		goto first_literal_run;
	}

	while (TEST_IP)
	{
		t = *ip++;
		if (t >= 16)
			goto match;
		/* a literal run */
		if (t == 0)
		{
			t = 15;
			while (*ip == 0)
				t += 255, ip++;
			t += *ip++;
		}
		/* copy literals */
		*op++ = *ip++; *op++ = *ip++; *op++ = *ip++;
	first_literal_run:
		do *op++ = *ip++; while (--t > 0);


		t = *ip++;

		if (t >= 16)
			goto match;
#if defined(LZO1X)
		m_pos = op - 1 - 0x800;
#elif defined(LZO1Y)
		m_pos = op - 1 - 0x400;
#endif
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;
		*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos++;
		goto match_done;


		/* handle matches */
		while (TEST_IP)
		{
			if (t < 16)						/* a M1 match */
			{
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				*op++ = *m_pos++; *op++ = *m_pos++;
			}
			else
			{
			match:
				if (t >= 64)				/* a M2 match */
				{
					m_pos = op - 1;
#if defined(LZO1X)
					m_pos -= (t >> 2) & 7;
					m_pos -= *ip++ << 3;
					t = (t >> 5) - 1;
#elif defined(LZO1Y)
					m_pos -= (t >> 2) & 3;
					m_pos -= *ip++ << 2;
					t = (t >> 4) - 3;
#endif
				}
				else if (t >= 32)			/* a M3 match */
				{
					t &= 31;
					if (t == 0)
					{
						t = 31;
						while (*ip == 0)
							t += 255, ip++;
						t += *ip++;
					}
					m_pos = op - 1;
					m_pos -= *ip++ >> 2;
					m_pos -= *ip++ << 6;
				}
				else						/* a M4 match */
				{
					m_pos = op;
					m_pos -= (t & 8) << 11;
					t &= 7;
					if (t == 0)
					{
						t = 7;
						while (*ip == 0)
							t += 255, ip++;
						t += *ip++;
					}
					m_pos -= *ip++ >> 2;
					m_pos -= *ip++ << 6;
					if (m_pos == op)
						goto eof_found;
					m_pos -= 0x4000;
				}
				*op++ = *m_pos++; *op++ = *m_pos++;
				do *op++ = *m_pos++; while (--t > 0);
			}

		match_done:
			t = ip[-2] & 3;
			if (t == 0)
				break;
			/* copy literals */
			do *op++ = *ip++; while (--t > 0);
			t = *ip++;
		}
	}

	/* ip == ip_end and no EOF code was found */

	//Unreachable - ST 9/5/96 5:07PM
	//*out_len = op - out;
	//return (ip == ip_end ? LZO_E_EOF_NOT_FOUND : LZO_E_ERROR);

eof_found:
	assert(t == 1);
	*out_len = op - out;
	return (ip == ip_end ? LZO_E_OK : LZO_E_ERROR);
}