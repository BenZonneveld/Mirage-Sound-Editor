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

#ifndef H_UTIL
#define H_UTIL

#include <stdio.h>
//#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <string.h>

#include "dsp.h"

signed long int quiet;

extern void win_return();
extern signed long int gettime();
extern inline double roundoff(double x);
extern inline signed long int roundup(double x);
extern float getfloat();
extern inline signed long int smallprimes(signed long int x);
extern inline signed long int nextsprime(signed long int x);
extern inline double log_b(double x);
extern inline unsigned long int rand_u32();
extern inline double dblrand();
extern inline unsigned int fread_le_short(FILE *file);
extern inline unsigned long int fread_le_word(FILE *file);
extern inline void fwrite_le_short(unsigned int s, FILE *file);
extern inline void fwrite_le_word(unsigned long int w, FILE *file);
extern char *getstring();
extern unsigned long int str_isnumber(char *string);

#endif