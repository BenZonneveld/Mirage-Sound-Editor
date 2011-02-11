/*
 * Copyright (c) 1997-1999 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sun Nov  7 20:43:52 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -real2hc 1 */

/*
 * This function contains 0 FP additions, 0 FP multiplications,
 * (or, 0 additions, 0 multiplications, 0 fused multiply/add),
 * 1 stack variables, and 2 memory accesses
 */

/*
 * Generator Id's : 
 * $Id: frc_1.c,v 1.1.1.1 2006/05/12 15:14:56 veplaini Exp $
 * $Id: frc_1.c,v 1.1.1.1 2006/05/12 15:14:56 veplaini Exp $
 * $Id: frc_1.c,v 1.1.1.1 2006/05/12 15:14:56 veplaini Exp $
 */

void fftw_real2hc_1(const fftw_real *input, fftw_real *real_output, fftw_real *imag_output, int istride, int real_ostride, int imag_ostride)
{
     fftw_real tmp1;
     ASSERT_ALIGNED_DOUBLE;
     tmp1 = input[0];
     real_output[0] = tmp1;
}

fftw_codelet_desc fftw_real2hc_1_desc =
{
     "fftw_real2hc_1",
     (void (*)()) fftw_real2hc_1,
     1,
     FFTW_FORWARD,
     FFTW_REAL2HC,
     24,
     0,
     (const int *) 0,
};
