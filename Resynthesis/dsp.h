/* The Analysis & Resynthesis Sound Spectrograph
Copyright (C) 2005-2008 Michel Rouzic

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/

#ifndef H_DSP
#define H_DSP

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed int int16_t;
typedef unsigned int uint16_t;
typedef signed long int int32_t;
typedef unsigned long int uint32_t;
typedef signed long long int int64_t;
typedef unsigned long long int uint64_t; 

#include <stdio.h>
//#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include <float.h>
#include <time.h>
#include <string.h>

#include "util.h"

#define PI_D            3.1415926535897932
#define LOGBASE_D       2
#define LOOP_SIZE_SEC_D 10.0
#define BMSQ_LUT_SIZE_D 16000

double pi;
double LOGBASE;                 // Base for log() operations. Anything other than 2 isn't really supported
#define TRANSITION_BW_SYNT 16.0         // defines the transition bandwidth for the low-pass filter on the envelopes during synthesisation
double LOOP_SIZE_SEC;           // size of the noise loops in seconds
signed long int BMSQ_LUT_SIZE;          // defines the number of elements in the Blackman Square look-up table. It's best to make it small enough to be entirely cached

signed long int clocka;

extern void fft(double *in, double *out, signed long int N, unsigned char method);
extern void normi(double **s, signed long int xs, signed long int ys, double ratio);
extern double *freqarray(double basefreq, signed long int bands, double bandsperoctave);
extern double *blackman_downsampling(double *in, signed long int Mi, signed long int Mo);
extern double *bmsq_lut(signed long int size);
extern void blackman_square_interpolation(double *in, double *out, signed long int Mi, signed long int Mo, double *lut, signed long int lut_size);
extern double **anal(double *s, signed long int samplecount, signed long int samplerate, signed long int *Xsize, signed long int bands, double bpo, double pixpersec, double basefreq);
extern double *wsinc_max(signed long int length, double bw);
extern double *synt_sine(double **d, signed long int Xsize, signed long int bands, signed long int *samplecount, signed long int samplerate, double basefreq, double pixpersec, double bpo);
extern double *synt_noise(double **d, signed long int Xsize, signed long int bands, signed long int *samplecount, signed long int samplerate, double basefreq, double pixpersec, double bpo);
extern void brightness_control(double **image, signed long int width, signed long int height, double ratio);

#endif
