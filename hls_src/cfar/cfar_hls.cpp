#include "cfar_hls.h"



/**
 *  	Top for HW CFAR IP core
 */
void CFAR_TOP(stream<stream_1ch> &in_stream, stream<stream_1ch> &out_stream)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis port=in_stream
#pragma HLS INTERFACE axis port=out_stream

	wrapper_cfar<uint32_t>(in_stream, out_stream);

}

/**
 *  	Top for testing sorting algorithm
 */
void SORT_TOP(uint32_t x[REFWIND], uint32_t y[REFWIND])
{
	sort_insert<uint32_t>(x, y);

}
