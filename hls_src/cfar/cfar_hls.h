#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "parameters.h"
#include <stdint.h>

using namespace hls;

/* ****************************** DEFINES ************************************** */

#define MAX_VALUE      		0x1FFFFFFF


typedef ap_axiu<32, 0, 0, 0> stream_1ch;

/* ****************************** STRUCTURES *********************************** */


/* ****************************** FUNCTIONS ************************************ */


/* ****************************** TOP FUNCTIONS DECLARATION ******************** */

void CFAR_TOP(stream<stream_1ch> &in_stream, stream<stream_1ch> &out_stream);

void SORT_TOP(uint32_t x[REFWIND], uint32_t y[REFWIND]);

/* ****************************** C++ TEMPLATES ********************************* */


/*
 * 	reading a sample from axis interface
 */
template <typename T>
T read_stream(stream_1ch const &e)
{
#pragma HLS INLINE
	union
	{
		int ival;
		T oval;
	} converter;
	converter.ival = e.data;
	T ret = converter.oval;

	return ret;
} // read_stream

template <typename T>
void pop_input(stream<stream_1ch> &in_stream,T y[NPOINTS])
{
#pragma HLS INLINE
	pin_L:for(uint16_t idx = 0; idx <  NPOINTS; idx ++)
		y[idx] = read_stream<T>(in_stream.read());
} // pop_input


/*
 * 	writing a sample to axis interface
 */
template <typename T>
stream_1ch write_stream(T const &v, bool last = false)
{
#pragma HLS INLINE
	stream_1ch e;

	union
	{
		int oval;
		T ival;
	} converter;
	converter.ival = v;
	e.data = converter.oval;
	e.last = last ? 1 : 0;

	return e;
}

template <typename T>
void push_output(stream<stream_1ch> &out_stream,T y[NPOINTS])
{
#pragma HLS INLINE
	pout_L:for(uint16_t idx = 0; idx <  NPOINTS; idx ++)
		out_stream.write(write_stream<T>(y[idx], (idx == NPOINTS - 1)));
} // push_output


template <typename T>
void pe(T din, T *staticvar, T *dout)
{
#pragma HLS INLINE
	if(din > *staticvar)
	{
		*dout = *staticvar;
		*staticvar = din;
	}
	else
		*dout = din;
}

template <typename T>
void sort_insert(T x[REFWIND], T y[REFWIND])
{
	static T d[REFWIND];
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=d
	static T s[REFWIND];
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=s
	L0_SORT:for(char idx = 0; idx < REFWIND; idx ++)
	{
#pragma HLS UNROLL
		d[idx] = 0;
		s[idx] = 0;
	}

	L1_SORT:for(T idx = 0; idx < 2*REFWIND; idx ++) 					// pipelined loop (2*REFWIND clocks)
	{
		if(idx < REFWIND)
			pe<T>(x[idx], &s[0], &d[0]);
		else
			pe<T>(MAX_VALUE, &s[0], &d[0]);

		L2_SORT:for (T idx_2 = 0; idx_2 < REFWIND-1; idx_2 ++) 			// fully unrolled (has REFWIND number of PE instances)
			pe<T>(d[idx_2], &s[idx_2 + 1], &d[idx_2 + 1]);

		if(idx >= REFWIND)
			y[idx - REFWIND] = d[REFWIND-1];
	} // for
}



template <typename T>
void cfar(T x[NPOINTS], T y[NPOINTS])
{
	static T x_sort[REFWIND];
#pragma HLS BIND_STORAGE variable=x_sort type=ram_t2p impl=bram
	static T y_sort[REFWIND];
#pragma HLS BIND_STORAGE variable=y_sort type=ram_t2p impl=bram
	T tmp_0, tmp_1;
#pragma HLS BIND_OP variable=tmp_0 op=mul impl=dsp 						// DSP may be omitted

	L0_CFAR:for (uint16_t idx =  0; idx < NPOINTS; idx ++) 				// cannot be pipelined since has L1_CFAR and sort_insert<T> in a sequence
	{

		if(idx >=  REFWIND / 2 && idx < (NPOINTS - REFWIND / 2))
		{
			L1_CFAR:for(char t1 = 0; t1 < REFWIND / 2; t1 ++) 			// pipelined loop (REFWIND / 2 clocks)
			{
#pragma HLS PIPELINE
				x_sort[t1] 					= x[idx - t1 - 1];
				x_sort[t1 + REFWIND / 2] 	= x[idx + t1];
			}

			sort_insert<T>(x_sort, y_sort);

			tmp_0  		= ((T)Z_COEF  * y_sort[KTH_CELL]) >> 10;
			y[idx] 		= tmp_0; 										// here the threshold itself instead of decision is provided
		}
		else
			y[idx] = 0;
	}
}




template <typename T>
void wrapper_cfar(stream<stream_1ch> &in_stream, stream<stream_1ch> &out_stream)
{
	T x[NPOINTS], y[NPOINTS];
#pragma HLS BIND_STORAGE variable=x type=ram_t2p impl=bram
#pragma HLS BIND_STORAGE variable=y type=ram_t2p impl=bram
	pop_input  	<T>( in_stream, x);
	cfar		<T>(x, y);
	push_output <T>(out_stream, y);
}
