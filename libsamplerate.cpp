/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include	"stdafx.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"config.h"
#include	"Mirage Editor.h"
#include	"samplerate.h"
#include	"common.h"
#include	"float_cast.h"

static int linear_process (SRC_PRIVATE *psrc, SRC_DATA *data) ;
static void linear_reset (SRC_PRIVATE *psrc) ;
static int psrc_set_converter (SRC_PRIVATE	*psrc, int converter_type) ;

/*========================================================================================
*/

#define	LINEAR_MAGIC_MARKER	MAKE_MAGIC ('l', 'i', 'n', 'e', 'a', 'r')

#define	SRC_DEBUG	0

typedef struct
{	int		linear_magic_marker ;
	int		channels ;
	long	in_count, in_used ;
	long	out_count, out_gen ;
	float	last_value [1] ;
} LINEAR_DATA ;

/*----------------------------------------------------------------------------------------
*/

/*------------src_sinc-----------*/
#define	SINC_MAGIC_MARKER	MAKE_MAGIC (' ', 's', 'i', 'n', 'c', ' ')

#define	ARRAY_LEN(x)		((int) (sizeof (x) / sizeof ((x) [0])))

/*========================================================================================
**	Macros for handling the index into the array for the filter.
**	Double precision floating point is not accurate enough so use a 64 bit
**	fixed point value instead. SHIFT_BITS (current value of 48) is the number
**	of bits to the right of the decimal point.
**	The rest of the macros are for retrieving the fractional and integer parts
**	and for converting floats and ints to the fixed point format or from the
**	fixed point type back to integers and floats.
*/

#define MAKE_INCREMENT_T(x) 	((increment_t) (x))

#define	SHIFT_BITS				16
#define	FP_ONE					((double) (((increment_t) 1) << SHIFT_BITS))

#define	DOUBLE_TO_FP(x)			(lrint ((x) * FP_ONE))
#define	INT_TO_FP(x)			(((increment_t) (x)) << SHIFT_BITS)

#define	FP_FRACTION_PART(x)		((x) & ((((increment_t) 1) << SHIFT_BITS) - 1))
#define	FP_INTEGER_PART(x)		((x) & (((increment_t) -1) << SHIFT_BITS))

#define	FP_TO_INT(x)			(((x) >> SHIFT_BITS))
#define	FP_TO_DOUBLE(x)			(FP_FRACTION_PART (x) / FP_ONE)

/*========================================================================================
*/

typedef int32_t increment_t ;
typedef float	coeff_t ;

enum
{
	STATE_BUFFER_START	= 101,
	STATE_DATA_CONTINUE	= 102,
	STATE_BUFFER_END	= 103,
	STATE_FINISHED
} ;

typedef struct
{	int		sinc_magic_marker ;

	int		channels ;
	long	in_count, in_used ;
	long	out_count, out_gen ;

	int		coeff_half_len, index_inc ;
	int		has_diffs ;

	double	src_ratio, input_index ;

	int		coeff_len ;
	coeff_t const	*coeffs ;

	int		b_current, b_end, b_real_end, b_len ;
	float	buffer [1] ;
} SINC_FILTER ;

static int sinc_process (SRC_PRIVATE *psrc, SRC_DATA *data) ;

static double calc_output (SINC_FILTER *filter, increment_t increment, increment_t start_filter_index, int ch) ;

static void prepare_data (SINC_FILTER *filter, SRC_DATA *data, int half_filter_chan_len) ;

static void sinc_reset (SRC_PRIVATE *psrc) ;

static coeff_t const high_qual_coeffs [] =
{
#include "high_qual_coeffs.h"
} ; /* high_qual_coeffs */

static coeff_t const mid_qual_coeffs [] =
{
#include "mid_qual_coeffs.h"
} ; /* mid_qual_coeffs */

static coeff_t const fastest_coeffs [] =
{
#include "fastest_coeffs.h"
} ; /* fastest_coeffs */

/*----------------------------------------------------------------------------------------
*/

static int zoh_process (SRC_PRIVATE *psrc, SRC_DATA *data) ;
static void zoh_reset (SRC_PRIVATE *psrc) ;

/*========================================================================================
*/

#define	ZOH_MAGIC_MARKER	MAKE_MAGIC ('s', 'r', 'c', 'z', 'o', 'h')

typedef struct
{	int		zoh_magic_marker ;
	int		channels ;
	long	in_count, in_used ;
	long	out_count, out_gen ;
	float	last_value [1] ;
} ZOH_DATA ;

/*----------------------------------------------------------------------------------------
*/

static int
zoh_process (SRC_PRIVATE *psrc, SRC_DATA *data)
{	ZOH_DATA 	*zoh ;
	double		src_ratio, input_index ;
	int			ch ;

	if (psrc->private_data == NULL)
		return SRC_ERR_NO_PRIVATE ;

	progress.Bar.SetRange((short)0,(short)data->output_frames);

	zoh = (ZOH_DATA*) psrc->private_data ;

	zoh->in_count = data->input_frames * zoh->channels ;
	zoh->out_count = data->output_frames * zoh->channels ;
	zoh->in_used = zoh->out_gen = 0 ;

	src_ratio = psrc->last_ratio ;
	input_index = psrc->last_position ;

	/* Calculate samples before first sample in input array. */
	while (input_index < 1.0 && zoh->out_gen < zoh->out_count)
	{
		if (zoh->in_used + zoh->channels * input_index >= zoh->in_count)
			break ;

		if (fabs (psrc->last_ratio - data->src_ratio) > SRC_MIN_RATIO_DIFF)
			src_ratio = psrc->last_ratio + zoh->out_gen * (data->src_ratio - psrc->last_ratio) / (zoh->out_count - 1) ;

		for (ch = 0 ; ch < zoh->channels ; ch++)
		{	data->data_out [zoh->out_gen] = zoh->last_value [ch] ;
			zoh->out_gen ++ ;
			} ;

		/* Figure out the next index. */
		input_index += 1.0 / src_ratio ;
		} ;

	zoh->in_used += zoh->channels * lrint (floor (input_index)) ;
	input_index -= floor (input_index) ;

	/* Main processing loop. */
	while (zoh->out_gen < zoh->out_count && zoh->in_used + zoh->channels * input_index <= zoh->in_count)
	{
		if (fabs (psrc->last_ratio - data->src_ratio) > SRC_MIN_RATIO_DIFF)
			src_ratio = psrc->last_ratio + zoh->out_gen * (data->src_ratio - psrc->last_ratio) / (zoh->out_count - 1) ;

		for (ch = 0 ; ch < zoh->channels ; ch++)
		{	data->data_out [zoh->out_gen] = data->data_in [zoh->in_used - zoh->channels + ch] ;
			zoh->out_gen ++ ;
			} ;

		/* Figure out the next index. */
		input_index += 1.0 / src_ratio ;

		zoh->in_used += zoh->channels * lrint (floor (input_index)) ;
		input_index -= floor (input_index) ;
		
		progress.progress(zoh->out_gen);
		} ;

	if (zoh->in_used > zoh->in_count)
	{	input_index += zoh->in_used - zoh->in_count ;
		zoh->in_used = zoh->in_count ;
		} ;

	psrc->last_position = input_index ;

	if (zoh->in_used > 0)
		for (ch = 0 ; ch < zoh->channels ; ch++)
			zoh->last_value [ch] = data->data_in [zoh->in_used - zoh->channels + ch] ;

	/* Save current ratio rather then target ratio. */
	psrc->last_ratio = src_ratio ;

	data->input_frames_used = zoh->in_used / zoh->channels ;
	data->output_frames_gen = zoh->out_gen / zoh->channels ;

	return SRC_ERR_NO_ERROR ;
} /* zoh_process */

/*------------------------------------------------------------------------------
*/

const char*
zoh_get_name (int src_enum)
{
	if (src_enum == SRC_ZERO_ORDER_HOLD)
		return "ZOH Interpolator" ;

	return NULL ;
} /* zoh_get_name */

const char*
zoh_get_description (int src_enum)
{
	if (src_enum == SRC_ZERO_ORDER_HOLD)
		return "Zero order hold interpolator, very fast, poor quality." ;

	return NULL ;
} /* zoh_get_descrition */

int
zoh_set_converter (SRC_PRIVATE *psrc, int src_enum)
{	ZOH_DATA *zoh = NULL ;

	if (src_enum != SRC_ZERO_ORDER_HOLD)
		return SRC_ERR_BAD_CONVERTER ;

	if (psrc->private_data != NULL)
	{	zoh = (ZOH_DATA*) psrc->private_data ;
		if (zoh->zoh_magic_marker != ZOH_MAGIC_MARKER)
		{	free (psrc->private_data) ;
			psrc->private_data = NULL ;
			} ;
		} ;

	if (psrc->private_data == NULL)
	{	zoh = (ZOH_DATA *)calloc (1, sizeof (*zoh) + psrc->channels * sizeof (float)) ;
		if (zoh == NULL)
			return SRC_ERR_MALLOC_FAILED ;
		psrc->private_data = zoh ;
		} ;

	zoh->zoh_magic_marker = ZOH_MAGIC_MARKER ;
	zoh->channels = psrc->channels ;

	psrc->process = zoh_process ;
	psrc->reset = zoh_reset ;

	zoh_reset (psrc) ;

	return SRC_ERR_NO_ERROR ;
} /* zoh_set_converter */

/*===================================================================================
*/

static void
zoh_reset (SRC_PRIVATE *psrc)
{	ZOH_DATA *zoh ;

	zoh = (ZOH_DATA*) psrc->private_data ;
	if (zoh == NULL)
		return ;

	zoh->channels = psrc->channels ;
	memset (zoh->last_value, 0, sizeof (zoh->last_value [0]) * zoh->channels) ;

	return ;
} /* zoh_reset */

const char*
sinc_get_name (int src_enum)
{
	switch (src_enum)
	{	case SRC_SINC_BEST_QUALITY :
			return "Best Sinc Interpolator" ;

		case SRC_SINC_MEDIUM_QUALITY :
			return "Medium Sinc Interpolator" ;

		case SRC_SINC_FASTEST :
			return "Fastest Sinc Interpolator" ;
		} ;

	return NULL ;
} /* sinc_get_descrition */

const char*
sinc_get_description (int src_enum)
{
	switch (src_enum)
	{	case SRC_SINC_BEST_QUALITY :
			return "Band limited sinc interpolation, best quality, 97dB SNR, 96% BW." ;

		case SRC_SINC_MEDIUM_QUALITY :
			return "Band limited sinc interpolation, medium quality, 97dB SNR, 90% BW." ;

		case SRC_SINC_FASTEST :
			return "Band limited sinc interpolation, fastest, 97dB SNR, 80% BW." ;
		} ;

	return NULL ;
} /* sinc_get_descrition */

int
sinc_set_converter (SRC_PRIVATE *psrc, int src_enum)
{	SINC_FILTER *filter, temp_filter ;
	int count, bits ;

	/* Quick sanity check. */
	if (SHIFT_BITS >= sizeof (increment_t) * 8 - 1)
		return SRC_ERR_SHIFT_BITS ;

	if (psrc->private_data != NULL)
	{	filter = (SINC_FILTER*) psrc->private_data ;
		if (filter->sinc_magic_marker != SINC_MAGIC_MARKER)
		{	free (psrc->private_data) ;
			psrc->private_data = NULL ;
			} ;
		} ;

	memset (&temp_filter, 0, sizeof (temp_filter)) ;

	temp_filter.sinc_magic_marker = SINC_MAGIC_MARKER ;
	temp_filter.channels = psrc->channels ;

	psrc->process = sinc_process ;
	psrc->reset = sinc_reset ;

	switch (src_enum)
	{	case SRC_SINC_BEST_QUALITY :
				temp_filter.coeffs = high_qual_coeffs ;
				temp_filter.coeff_half_len = ARRAY_LEN (high_qual_coeffs) - 1 ;
				temp_filter.index_inc = 128 ;
				temp_filter.has_diffs = SRC_FALSE ;
				temp_filter.coeff_len = ARRAY_LEN (high_qual_coeffs) ;
				break ;

		case SRC_SINC_MEDIUM_QUALITY :
				temp_filter.coeffs = mid_qual_coeffs ;
				temp_filter.coeff_half_len = ARRAY_LEN (mid_qual_coeffs) - 1 ;
				temp_filter.index_inc = 128 ;
				temp_filter.has_diffs = SRC_FALSE ;
				temp_filter.coeff_len = ARRAY_LEN (mid_qual_coeffs) ;
				break ;

		case SRC_SINC_FASTEST :
				temp_filter.coeffs = fastest_coeffs ;
				temp_filter.coeff_half_len = ARRAY_LEN (fastest_coeffs) - 1 ;
				temp_filter.index_inc = 128 ;
				temp_filter.has_diffs = SRC_FALSE ;
				temp_filter.coeff_len = ARRAY_LEN (fastest_coeffs) ;
				break ;

		default :
				return SRC_ERR_BAD_CONVERTER ;
		} ;

	/*
	** FIXME : This needs to be looked at more closely to see if there is
	** a better way. Need to look at prepare_data () at the same time.
	*/

	temp_filter.b_len = 1000 + 2 * lrint (0.5 + temp_filter.coeff_len / (temp_filter.index_inc * 1.0) * SRC_MAX_RATIO) ;
	temp_filter.b_len *= temp_filter.channels ;

	if ((filter = (SINC_FILTER *)calloc (1, sizeof (SINC_FILTER) + sizeof (filter->buffer [0]) * (temp_filter.b_len + temp_filter.channels))) == NULL)
		return SRC_ERR_MALLOC_FAILED ;

	*filter = temp_filter ;
	memset (&temp_filter, 0xEE, sizeof (temp_filter)) ;

	psrc->private_data = filter ;

	sinc_reset (psrc) ;

	count = filter->coeff_half_len ;
	for (bits = 0 ; (1 << bits) < count ; bits++)
		count |= (1 << bits) ;

	if (bits + SHIFT_BITS - 1 >= (int) (sizeof (increment_t) * 8))
		return SRC_ERR_FILTER_LEN ;

	return SRC_ERR_NO_ERROR ;
} /* sinc_set_converter */

static void
sinc_reset (SRC_PRIVATE *psrc)
{	SINC_FILTER *filter ;

	filter = (SINC_FILTER*) psrc->private_data ;
	if (filter == NULL)
		return ;

	filter->b_current = filter->b_end = 0 ;
	filter->b_real_end = -1 ;

	filter->src_ratio = filter->input_index = 0.0 ;

	memset (filter->buffer, 0, filter->b_len * sizeof (filter->buffer [0])) ;

	/* Set this for a sanity check */
	memset (filter->buffer + filter->b_len, 0xAA, filter->channels * sizeof (filter->buffer [0])) ;
} /* sinc_reset */

/*========================================================================================
**	Beware all ye who dare pass this point. There be dragons here.
*/

static int
sinc_process (SRC_PRIVATE *psrc, SRC_DATA *data)
{	SINC_FILTER *filter ;
	double		input_index, src_ratio, count, float_increment, terminate, rem ;
	increment_t	increment, start_filter_index ;
	int			half_filter_chan_len, samples_in_hand, ch ;

	if (psrc->private_data == NULL)
		return SRC_ERR_NO_PRIVATE ;

	progress.Bar.SetRange((short)0,(short)data->output_frames);

	filter = (SINC_FILTER*) psrc->private_data ;

	/* If there is not a problem, this will be optimised out. */
	if (sizeof (filter->buffer [0]) != sizeof (data->data_in [0]))
		return SRC_ERR_SIZE_INCOMPATIBILITY ;

	filter->in_count = data->input_frames * filter->channels ;
	filter->out_count = data->output_frames * filter->channels ;
	filter->in_used = filter->out_gen = 0 ;

	src_ratio = psrc->last_ratio ;

	/* Check the sample rate ratio wrt the buffer len. */
	count = (filter->coeff_half_len + 2.0) / filter->index_inc ;
	if (MIN (psrc->last_ratio, data->src_ratio) < 1.0)
		count /= MIN (psrc->last_ratio, data->src_ratio) ;

	/* Maximum coefficientson either side of center point. */
	half_filter_chan_len = filter->channels * (lrint (count) + 1) ;

	input_index = psrc->last_position ;
	float_increment = filter->index_inc ;

	rem = fmod (input_index, 1.0) ;
	filter->b_current = (filter->b_current + filter->channels * lrint (input_index - rem)) % filter->b_len ;
	input_index = rem ;

	terminate = 1.0 / src_ratio + 1e-20 ;

	/* Main processing loop. */
	while (filter->out_gen < filter->out_count)
	{
		/* Need to reload buffer? */
		samples_in_hand = (filter->b_end - filter->b_current + filter->b_len) % filter->b_len ;

		if (samples_in_hand <= half_filter_chan_len)
		{	prepare_data (filter, data, half_filter_chan_len) ;

			samples_in_hand = (filter->b_end - filter->b_current + filter->b_len) % filter->b_len ;
			if (samples_in_hand <= half_filter_chan_len)
				break ;
		} ;

		/* This is the termination condition. */
		if (filter->b_real_end >= 0)
		{	if (filter->b_current + input_index + terminate >= filter->b_real_end)
				break ;
			} ;

		if (fabs (psrc->last_ratio - data->src_ratio) > 1e-10)
			src_ratio = psrc->last_ratio + filter->out_gen * (data->src_ratio - psrc->last_ratio) / (filter->out_count - 1) ;

		float_increment = filter->index_inc * 1.0 ;
		if (src_ratio < 1.0)
			float_increment = filter->index_inc * src_ratio ;

		increment = DOUBLE_TO_FP (float_increment) ;

		start_filter_index = DOUBLE_TO_FP (input_index * float_increment) ;

		for (ch = 0 ; ch < filter->channels ; ch++)
		{	data->data_out [filter->out_gen] = (float)((float_increment / filter->index_inc) *
											calc_output (filter, increment, start_filter_index, ch)) ;
			filter->out_gen ++ ;
			} ;

		/* Figure out the next index. */
		input_index += 1.0 / src_ratio ;
		rem = fmod (input_index, 1.0) ;

		filter->b_current = (filter->b_current + filter->channels * lrint (input_index - rem)) % filter->b_len ;
		input_index = rem ;

		progress.progress(filter->out_gen);
		} ;

	psrc->last_position = input_index ;

	/* Save current ratio rather then target ratio. */
	psrc->last_ratio = src_ratio ;

	data->input_frames_used = filter->in_used / filter->channels ;
	data->output_frames_gen = filter->out_gen / filter->channels ;

	return SRC_ERR_NO_ERROR ;
} /* sinc_process */

/*----------------------------------------------------------------------------------------
*/

static void
prepare_data (SINC_FILTER *filter, SRC_DATA *data, int half_filter_chan_len)
{	int len = 0 ;

	if (filter->b_real_end >= 0)
		return ;	/* This doesn't make sense, so return. */

	if (filter->b_current == 0)
	{	/* Initial state. Set up zeros at the start of the buffer and
		** then load new data after that.
		*/
		len = filter->b_len - 2 * half_filter_chan_len ;

		filter->b_current = filter->b_end = half_filter_chan_len ;
		}
	else if (filter->b_end + half_filter_chan_len + filter->channels < filter->b_len)
	{	/*  Load data at current end position. */
		len = MAX (filter->b_len - filter->b_current - half_filter_chan_len, 0) ;
		}
	else
	{	/* Move data at end of buffer back to the start of the buffer. */
		len = filter->b_end - filter->b_current ;
		memmove (filter->buffer, filter->buffer + filter->b_current - half_filter_chan_len,
						(half_filter_chan_len + len) * sizeof (filter->buffer [0])) ;

		filter->b_current = half_filter_chan_len ;
		filter->b_end = filter->b_current + len ;

		/* Now load data at current end of buffer. */
		len = MAX (filter->b_len - filter->b_current - half_filter_chan_len, 0) ;
		} ;

	len = MIN (filter->in_count - filter->in_used, len) ;
	len -= (len % filter->channels) ;

	memcpy(filter->buffer + filter->b_end, data->data_in + filter->in_used,
						len * sizeof (filter->buffer [0])) ;

	filter->b_end += len ;
	filter->in_used += len ;

	if (filter->in_used == filter->in_count &&
			filter->b_end - filter->b_current < 2 * half_filter_chan_len && data->end_of_input)
	{	/* Handle the case where all data in the current buffer has been
		** consumed and this is the last buffer.
		*/

		if (filter->b_len - filter->b_end < half_filter_chan_len + 5)
		{	/* If necessary, move data down to the start of the buffer. */
			len = filter->b_end - filter->b_current ;
			memmove (filter->buffer, filter->buffer + filter->b_current - half_filter_chan_len,
							(half_filter_chan_len + len) * sizeof (filter->buffer [0])) ;

			filter->b_current = half_filter_chan_len ;
			filter->b_end = filter->b_current + len ;
			} ;

		filter->b_real_end = filter->b_end ;
		len = half_filter_chan_len + 5 ;

		memset (filter->buffer + filter->b_end, 0, len * sizeof (filter->buffer [0])) ;
		filter->b_end += len ;
		} ;

	return ;
} /* prepare_data */


static double
calc_output (SINC_FILTER *filter, increment_t increment, increment_t start_filter_index, int ch)
{	double		fraction, left, right, icoeff ;
	increment_t	filter_index, max_filter_index ;
	int			data_index, coeff_count, indx ;

	/* Convert input parameters into fixed point. */
	max_filter_index = INT_TO_FP (filter->coeff_half_len) ;

	/* First apply the left half of the filter. */
	filter_index = start_filter_index ;
	coeff_count = (max_filter_index - filter_index) / increment ;
	filter_index = filter_index + coeff_count * increment ;
	data_index = filter->b_current - filter->channels * coeff_count ;

	left = 0.0 ;
	do
	{	fraction = FP_TO_DOUBLE (filter_index) ;
		indx = FP_TO_INT (filter_index) ;

		icoeff = filter->coeffs [indx] + fraction * (filter->coeffs [indx + 1] - filter->coeffs [indx]) ;

		left += icoeff * filter->buffer [data_index + ch] ;

		filter_index -= increment ;
		data_index = data_index + filter->channels ;
		}
	while (filter_index >= MAKE_INCREMENT_T (0)) ;

	/* Now apply the right half of the filter. */
	filter_index = increment - start_filter_index ;
	coeff_count = (max_filter_index - filter_index) / increment ;
	filter_index = filter_index + coeff_count * increment ;
	data_index = filter->b_current + filter->channels * (1 + coeff_count) ;

	right = 0.0 ;
	do
	{	fraction = FP_TO_DOUBLE (filter_index) ;
		indx = FP_TO_INT (filter_index) ;

		icoeff = filter->coeffs [indx] + fraction * (filter->coeffs [indx + 1] - filter->coeffs [indx]) ;

		right += icoeff * filter->buffer [data_index + ch] ;

		filter_index -= increment ;
		data_index = data_index - filter->channels ;
		}
	while (filter_index > MAKE_INCREMENT_T (0)) ;

	return (left + right) ;
} /* calc_output */

static int
linear_process (SRC_PRIVATE *psrc, SRC_DATA *data)
{	LINEAR_DATA *linear ;
	double		src_ratio, input_index ;
	int			ch ;

	if (psrc->private_data == NULL)
		return SRC_ERR_NO_PRIVATE ;

	progress.Bar.SetRange((short)0,(short)data->output_frames);

	linear = (LINEAR_DATA*) psrc->private_data ;

	linear->in_count = data->input_frames * linear->channels ;
	linear->out_count = data->output_frames * linear->channels ;
	linear->in_used = linear->out_gen = 0 ;

	src_ratio = psrc->last_ratio ;
	input_index = psrc->last_position ;

	/* Calculate samples before first sample in input array. */
	while (input_index < 1.0 && linear->out_gen < linear->out_count)
	{
		if (linear->in_used + linear->channels * input_index > linear->in_count)
			break ;

		if (fabs (psrc->last_ratio - data->src_ratio) > SRC_MIN_RATIO_DIFF)
			src_ratio = psrc->last_ratio + linear->out_gen * (data->src_ratio - psrc->last_ratio) / (linear->out_count - 1) ;

		for (ch = 0 ; ch < linear->channels ; ch++)
		{	data->data_out [linear->out_gen] = linear->last_value [ch] + (float)input_index *
										(data->data_in [ch] - linear->last_value [ch]) ;
			linear->out_gen ++ ;
			} ;

		/* Figure out the next index. */
		input_index += 1.0 / src_ratio ;
		} ;

	linear->in_used += linear->channels * lrint (floor (input_index)) ;
	input_index -= floor (input_index) ;

	/* Main processing loop. */
	while (linear->out_gen < linear->out_count && linear->in_used + linear->channels * input_index <= linear->in_count)
	{
		if (fabs (psrc->last_ratio - data->src_ratio) > SRC_MIN_RATIO_DIFF)
			src_ratio = psrc->last_ratio + linear->out_gen * (data->src_ratio - psrc->last_ratio) / (linear->out_count - 1) ;

		if (SRC_DEBUG && linear->in_used < linear->channels && input_index < 1.0)
		{	printf ("Whoops!!!!   in_used : %ld     channels : %d     input_index : %f\n", linear->in_used, linear->channels, input_index) ;
			exit (1) ;
			} ;

		/* Break here */
		for (ch = 0 ; ch < linear->channels ; ch++)
		{	
			data->data_out [linear->out_gen] = data->data_in [linear->in_used - linear->channels + ch] + (float)input_index *
						(data->data_in [linear->in_used + ch] - data->data_in [linear->in_used - linear->channels + ch]) ;
			linear->out_gen ++ ;
			} ;

		/* Figure out the next index. */
		input_index += 1.0 / src_ratio ;

		linear->in_used += linear->channels * lrint (floor (input_index)) ;
		input_index -= floor (input_index) ;
		progress.progress(linear->out_gen);
		} ;

	if (linear->in_used > linear->in_count)
	{	input_index += linear->in_used - linear->in_count ;
		linear->in_used = linear->in_count ;
		} ;

	psrc->last_position = input_index ;

	if (linear->in_used > 0)
		for (ch = 0 ; ch < linear->channels ; ch++)
			linear->last_value [ch] = data->data_in [linear->in_used - linear->channels + ch] ;

	/* Save current ratio rather then target ratio. */
	psrc->last_ratio = src_ratio ;

	data->input_frames_used = linear->in_used / linear->channels ;
	data->output_frames_gen = linear->out_gen / linear->channels ;

	return SRC_ERR_NO_ERROR ;
} /* linear_process */

/*------------------------------------------------------------------------------
*/

const char*
linear_get_name (int src_enum)
{
	if (src_enum == SRC_LINEAR)
		return "Linear Interpolator" ;

	return NULL ;
} /* linear_get_name */

const char*
linear_get_description (int src_enum)
{
	if (src_enum == SRC_LINEAR)
		return "Linear interpolator, very fast, poor quality." ;

	return NULL ;
} /* linear_get_descrition */

int
linear_set_converter (SRC_PRIVATE *psrc, int src_enum)
{	LINEAR_DATA *linear = NULL ;

	if (src_enum != SRC_LINEAR)
		return SRC_ERR_BAD_CONVERTER ;

	if (psrc->private_data != NULL)
	{	linear = (LINEAR_DATA*) psrc->private_data ;
		if (linear->linear_magic_marker != LINEAR_MAGIC_MARKER)
		{	free (psrc->private_data) ;
			psrc->private_data = NULL ;
			} ;
		} ;

	if (psrc->private_data == NULL)
	{	linear = (LINEAR_DATA *)calloc (1, sizeof (*linear) + psrc->channels * sizeof (float)) ;
		if (linear == NULL)
			return SRC_ERR_MALLOC_FAILED ;
		psrc->private_data = linear ;
		} ;

	linear->linear_magic_marker = LINEAR_MAGIC_MARKER ;
	linear->channels = psrc->channels ;

	psrc->process = linear_process ;
	psrc->reset = linear_reset ;

	linear_reset (psrc) ;

	return SRC_ERR_NO_ERROR ;
} /* linear_set_converter */

/*===================================================================================
*/

static void
linear_reset (SRC_PRIVATE *psrc)
{	LINEAR_DATA *linear = NULL ;

	linear = (LINEAR_DATA*) psrc->private_data ;
	if (linear == NULL)
		return ;

	memset (linear->last_value, 0, sizeof (linear->last_value [0]) * linear->channels) ;
} /* linear_reset */

SRC_STATE *
src_new (int converter_type, int channels, int *error)
{	SRC_PRIVATE	*psrc ;

	if (error)
		*error = SRC_ERR_NO_ERROR ;

	if (channels < 1)
	{	if (error)
			*error = SRC_ERR_BAD_CHANNEL_COUNT ;
		return NULL ;
		} ;

	if ((psrc = (SRC_PRIVATE *)calloc (1, sizeof (*psrc))) == NULL)
	{	if (error)
			*error = SRC_ERR_MALLOC_FAILED ;
		return NULL ;
		} ;

	psrc->channels = channels ;
	psrc->mode = SRC_MODE_PROCESS ;

	if (psrc_set_converter (psrc, converter_type) != SRC_ERR_NO_ERROR)
	{	if (error)
			*error = SRC_ERR_BAD_CONVERTER ;
		free (psrc) ;
		psrc = NULL ;
		} ;

	src_reset ((SRC_STATE*) psrc) ;

	return (SRC_STATE*) psrc ;
} /* src_new */

SRC_STATE*
src_callback_new (src_callback_t func, int converter_type, int channels, int *error, void* cb_data)
{	SRC_STATE	*src_state ;

	if (func == NULL)
	{	if (error)
			*error = SRC_ERR_BAD_CALLBACK ;
		return NULL ;
		} ;

	if (error != NULL)
		*error = 0 ;

	src_state = src_new (converter_type, channels, error) ;

	src_reset (src_state) ;

	((SRC_PRIVATE*) src_state)->mode = SRC_MODE_CALLBACK ;
	((SRC_PRIVATE*) src_state)->callback_func = func ;
	((SRC_PRIVATE*) src_state)->user_callback_data = cb_data ;

	return src_state ;
} /* src_callback_new */

SRC_STATE *
src_delete (SRC_STATE *state)
{	SRC_PRIVATE *psrc ;

	psrc = (SRC_PRIVATE*) state ;
	if (psrc)
	{	if (psrc->private_data)
			free (psrc->private_data) ;
		memset (psrc, 0, sizeof (SRC_PRIVATE)) ;
		free (psrc) ;
		} ;

	return NULL ;
} /* src_state */

int
src_process (SRC_STATE *state, SRC_DATA *data)
{	SRC_PRIVATE *psrc ;
	int error ;

	psrc = (SRC_PRIVATE*) state ;

	if (psrc == NULL)
		return SRC_ERR_BAD_STATE ;
	if (psrc->process == NULL)
		return SRC_ERR_BAD_PROC_PTR ;

	if (psrc->mode != SRC_MODE_PROCESS)
		return SRC_ERR_BAD_MODE ;

	/* Check for valid SRC_DATA first. */
	if (data == NULL)
		return SRC_ERR_BAD_DATA ;

	/* Check src_ratio is in range. */
	if (data->src_ratio < (1.0 / SRC_MAX_RATIO) || data->src_ratio > (1.0 * SRC_MAX_RATIO))
		return SRC_ERR_BAD_SRC_RATIO ;

	/* And that data_in and data_out are valid. */
	if (data->data_in == NULL || data->data_out == NULL)
		return SRC_ERR_BAD_DATA_PTR ;

	if (data->data_in == NULL)
		data->input_frames = 0 ;

	if (data->input_frames < 0)
		data->input_frames = 0 ;
	if (data->output_frames < 0)
		data->output_frames = 0 ;

	if (data->data_in < data->data_out)
	{	if (data->data_in + data->input_frames * psrc->channels > data->data_out)
		{	/*-printf ("\n\ndata_in: %p    data_out: %p\n",
				(void*) (data->data_in + data->input_frames * psrc->channels), (void*) data->data_out) ;-*/
			return SRC_ERR_DATA_OVERLAP ;
			} ;
		}
	else if (data->data_out + data->output_frames * psrc->channels > data->data_in)
	{	/*-printf ("\n\ndata_in : %p   ouput frames: %ld    data_out: %p\n", (void*) data->data_in, data->output_frames, (void*) data->data_out) ;

		printf ("data_out: %p (%p)    data_in: %p\n", (void*) data->data_out,
			(void*) (data->data_out + data->input_frames * psrc->channels), (void*) data->data_in) ;-*/
		return SRC_ERR_DATA_OVERLAP ;
		} ;

	/* Set the input and output counts to zero. */
	data->input_frames_used = 0 ;
	data->output_frames_gen = 0 ;

	/* Special case for when last_ratio has not been set. */
	if (psrc->last_ratio < (1.0 / SRC_MAX_RATIO))
		psrc->last_ratio = data->src_ratio ;

	/* Now process. */
	error = psrc->process (psrc, data) ;

	return error ;
} /* src_process */

long
src_callback_read (SRC_STATE *state, double src_ratio, long frames, float *data)
{	SRC_PRIVATE	*psrc ;
	SRC_DATA	src_data ;

	long	output_frames_gen ;
	int		error = 0 ;

	if (state == NULL)
		return 0 ;

	if (frames <= 0)
		return 0 ;

	psrc = (SRC_PRIVATE*) state ;

	if (psrc->mode != SRC_MODE_CALLBACK)
	{	psrc->error = SRC_ERR_BAD_MODE ;
		return 0 ;
		} ;

	if (psrc->callback_func == NULL)
	{	psrc->error = SRC_ERR_NULL_CALLBACK ;
		return 0 ;
		} ;

	memset (&src_data, 0, sizeof (src_data)) ;

	/* Check src_ratio is in range. */
	if (src_ratio < (1.0 / SRC_MAX_RATIO) || src_ratio > (1.0 * SRC_MAX_RATIO))
	{	psrc->error = SRC_ERR_BAD_SRC_RATIO ;
		return 0 ;
		} ;

	/* Switch modes temporarily. */
	src_data.src_ratio = src_ratio ;
	src_data.data_out = data ;
	src_data.output_frames = frames ;

	src_data.data_in = psrc->saved_data ;
	src_data.input_frames = psrc->saved_frames ;

	output_frames_gen = 0 ;
	while (output_frames_gen < frames)
	{
		if (src_data.input_frames == 0)
		{	float *ptr ;

			src_data.input_frames = psrc->callback_func (psrc->user_callback_data, &ptr) ;
			src_data.data_in = ptr ;

			if (src_data.input_frames == 0)
				src_data.end_of_input = 1 ;
			} ;

		/*
		** Now call process function. However, we need to set the mode
		** to SRC_MODE_PROCESS first and when we return set it back to
		** SRC_MODE_CALLBACK.
		*/
		psrc->mode = SRC_MODE_PROCESS ;
		error = src_process (state, &src_data) ;
		psrc->mode = SRC_MODE_CALLBACK ;

		if (error != 0)
			break ;

		src_data.data_in += src_data.input_frames_used * psrc->channels ;
		src_data.input_frames -= src_data.input_frames_used ;

		src_data.data_out += src_data.output_frames_gen * psrc->channels ;
		src_data.output_frames -= src_data.output_frames_gen ;

		output_frames_gen += src_data.output_frames_gen ;

		if (src_data.end_of_input == SRC_TRUE && src_data.output_frames_gen == 0)
			break ;
		} ;

	psrc->saved_data = src_data.data_in ;
	psrc->saved_frames = src_data.input_frames ;

	if (error != 0)
	{	psrc->error = error ;
	 	return 0 ;
		} ;

	return output_frames_gen ;
} /* src_callback_read */

/*==========================================================================
*/

int
src_set_ratio (SRC_STATE *state, double new_ratio)
{	SRC_PRIVATE *psrc ;

	psrc = (SRC_PRIVATE*) state ;

	if (psrc == NULL)
		return SRC_ERR_BAD_STATE ;
	if (psrc->process == NULL)
		return SRC_ERR_BAD_PROC_PTR ;

	psrc->last_ratio = new_ratio ;

	return SRC_ERR_NO_ERROR ;
} /* src_set_ratio */

int
src_reset (SRC_STATE *state)
{	SRC_PRIVATE *psrc ;

	if ((psrc = (SRC_PRIVATE*) state) == NULL)
		return SRC_ERR_BAD_STATE ;

	if (psrc->reset != NULL)
		psrc->reset (psrc) ;

	psrc->last_position = 0.0 ;
	psrc->last_ratio = 0.0 ;

	psrc->saved_data = NULL ;
	psrc->saved_frames = 0 ;

	psrc->error = SRC_ERR_NO_ERROR ;

	return SRC_ERR_NO_ERROR ;
} /* src_reset */

/*==============================================================================
**	Control functions.
*/

const char *
src_get_name (int converter_type)
{	const char *desc ;

	if ((desc = sinc_get_name (converter_type)) != NULL)
		return desc ;

	if ((desc = zoh_get_name (converter_type)) != NULL)
		return desc ;

	if ((desc = linear_get_name (converter_type)) != NULL)
		return desc ;

	return NULL ;
} /* src_get_name */

const char *
src_get_description (int converter_type)
{	const char *desc ;

	if ((desc = sinc_get_description (converter_type)) != NULL)
		return desc ;

	if ((desc = zoh_get_description (converter_type)) != NULL)
		return desc ;

	if ((desc = linear_get_description (converter_type)) != NULL)
		return desc ;

	return NULL ;
} /* src_get_description */

const char *
src_get_version (void)
{	return PACKAGE "-" VERSION ;
} /* src_get_version */

int
src_is_valid_ratio (double ratio)
{
	if (ratio < (1.0 / SRC_MAX_RATIO) || ratio > (1.0 * SRC_MAX_RATIO))
		return SRC_FALSE ;

	return SRC_TRUE ;
} /* src_is_valid_ratio */

/*==============================================================================
**	Error reporting functions.
*/

int
src_error (SRC_STATE *state)
{	if (state)
		return ((SRC_PRIVATE*) state)->error ;
	return SRC_ERR_NO_ERROR ;
} /* src_error */

const char*
src_strerror (int error)
{
	switch (error)
	{	case SRC_ERR_NO_ERROR :
				return "No error." ;
		case SRC_ERR_MALLOC_FAILED :
				return "Malloc failed." ;
		case SRC_ERR_BAD_STATE :
				return "SRC_STATE pointer is NULL." ;
		case SRC_ERR_BAD_DATA :
				return "SRC_DATA pointer is NULL." ;
		case SRC_ERR_BAD_DATA_PTR :
				return "SRC_DATA->data_out is NULL." ;
		case SRC_ERR_NO_PRIVATE :
				return "Internal error. No private data." ;
		case SRC_ERR_BAD_SRC_RATIO :
				return "SRC ratio outside [1/256, 256] range." ;
		case SRC_ERR_BAD_SINC_STATE :
				return "src_process() called without reset after end_of_input." ;
		case SRC_ERR_BAD_PROC_PTR :
				return "Internal error. No process pointer." ;
		case SRC_ERR_SHIFT_BITS :
				return "Internal error. SHIFT_BITS too large." ;
		case SRC_ERR_FILTER_LEN :
				return "Internal error. Filter length too large." ;
		case SRC_ERR_BAD_CONVERTER :
				return "Bad converter number." ;
		case SRC_ERR_BAD_CHANNEL_COUNT :
				return "Channel count must be >= 1." ;
		case SRC_ERR_SINC_BAD_BUFFER_LEN :
				return "Internal error. Bad buffer length. Please report this." ;
		case SRC_ERR_SIZE_INCOMPATIBILITY :
				return "Internal error. Input data / internal buffer size difference. Please report this." ;
		case SRC_ERR_BAD_PRIV_PTR :
				return "Internal error. Private pointer is NULL. Please report this." ;
		case SRC_ERR_DATA_OVERLAP :
				return "Input and output data arrays overlap." ;
		case SRC_ERR_BAD_CALLBACK :
				return "Supplied callback function pointer is NULL." ;
		case SRC_ERR_BAD_MODE :
				return "Calling mode differs from initialisation mode (ie process v callback)." ;
		case SRC_ERR_NULL_CALLBACK :
				return "Callback function pointer is NULL in src_callback_read ()." ;

		case SRC_ERR_MAX_ERROR :
				return "Placeholder. No error defined for this error number." ;

		default : 						break ;
		}

	return NULL ;
} /* src_strerror */

/*==============================================================================
**	Simple interface for performing a single conversion from input buffer to
**	output buffer at a fixed conversion ratio.
*/

int
src_simple (SRC_DATA *src_data, int converter, int channels)
{	SRC_STATE	*src_state ;
	int 		error ;

	if ((src_state = src_new (converter, channels, &error)) == NULL)
		return error ;

	src_data->end_of_input = 1 ; /* Only one buffer worth of input. */

	error = src_process (src_state, src_data) ;

	src_state = src_delete (src_state) ;

	return error ;
} /* src_simple */

void src_unchar_to_float_array (const unsigned char *in, float *out, int len)
{
	char NewValue;
	while (len)
	{
		len --;
		NewValue = in[len]-128;
		out[len] = float(NewValue / (1.0 * 0x80));
	}
	return;
} /* src_unchar_to_float_array */

void
src_float_to_unchar_array (const float *in, unsigned char *out, int len)
{	
	float scaled_value ;

	while (len)
	{
		len -- ;

		scaled_value = float((in [len]+1) * 127.5) ;
		if ( scaled_value < 0 )
		{
			scaled_value = 0;
		}
		if ( scaled_value > 255 )
		{
			scaled_value = 255;
		}
/*		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7F))
		{	out [len] = 127 ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10))
		{	out [len] = -127;
			continue ;
			} ;
*/
		out [len] = (unsigned char)(lrintf (scaled_value) /*- 128/* >> 8*/) ;
		} ;

} /* src_float_to_unchar_array */

void src_short_to_float_array (const short *in, float *out, int len)
{
	while (len)
	{	len -- ;
		out [len] = float(in [len] / (1.0 * 0x8000)) ;
		} ;

	return ;
} /* src_short_to_float_array */

void
src_float_to_short_array (const float *in, short *out, int len)
{	float scaled_value ;

	while (len)
	{	len -- ;

		scaled_value = float(in [len] * (8.0 * 0x10000000)) ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	out [len] = 32767 ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	out [len] = -32768 ;
			continue ;
			} ;

		out [len] = (short)(lrintf (scaled_value) >> 16) ;
		} ;

} /* src_float_to_short_array */

double apply_gain(float * data, long frames, int channels, double max, double gain)
{
	long k ;

	for (k = 0 ; k < frames * channels ; k++)
	{	data [k] *= float(gain) ;

		if (fabs (data [k]) > max)
			max = fabs (data [k]) ;
		} ;

	return max ;
} /* apply_gain */

/*==============================================================================
**	Private functions.
*/

static int
psrc_set_converter (SRC_PRIVATE	*psrc, int converter_type)
{
	if (sinc_set_converter (psrc, converter_type) == SRC_ERR_NO_ERROR)
		return SRC_ERR_NO_ERROR ;

	if (zoh_set_converter (psrc, converter_type) == SRC_ERR_NO_ERROR)
		return SRC_ERR_NO_ERROR ;

	if (linear_set_converter (psrc, converter_type) == SRC_ERR_NO_ERROR)
		return SRC_ERR_NO_ERROR ;

	return SRC_ERR_BAD_CONVERTER ;
} /* psrc_set_converter */
