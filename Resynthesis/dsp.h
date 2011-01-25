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

#include "dsp_types.h"

#define PI_D            3.1415926535897932
#define LOGBASE_D       2
#define LOOP_SIZE_SEC_D 0.5
#define BMSQ_LUT_SIZE_D 16000

/* From util.h */
extern signed long int gettime();
extern inline double roundoff(double x);
extern inline signed long int roundup(double x);
extern inline signed long int smallprimes(signed long int x);
extern inline signed long int nextsprime(signed long int x);
extern inline double log_b(double x);
extern inline unsigned long int rand_u32();
extern inline double dblrand();

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
extern void settingsinput(int32_t *bands, int32_t samplecount, int32_t samplerate, double *basefreq, double *maxfreq, double *pixpersec, double *bandsperoctave, int32_t Xsize, int32_t mode, double logbase);
extern double ** convolver(double **image,int32_t Xsize, int32_t Ysize, int *Ksize);
#endif
