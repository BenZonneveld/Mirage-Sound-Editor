 
////////////////////////////////////////////////////////////////////////
// This file is part of the SndObj library
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
//
// Copyright (c)Victor Lazzarini, 1997-2004
// See License.txt for a disclaimer of all warranties
// and licensing information
//
// PVConvol.h: interface for the PVConvol class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _PVConvol_H
#define _PVConvol_H


#include "SpecMult.h"

class PVConvol : public SpecMult  
{
 protected:
  float **m_frame;
  int m_framenos;
  int m_hopsize;
  int m_cur;

 public:
  int Set(char* mess, float value);
  void SetConvolTime(int convolframes);
  void SetHopsize(int hopsize){ m_hopsize = hopsize;
  SetConvolTime(m_framenos);
  }
  PVConvol();
  PVConvol(SndObj* input, int convolframes, int hopsize=DEF_VECSIZE, 
	 int vecsize=DEF_FFTSIZE, float sr=DEF_SR);
  ~PVConvol();
  short DoProcess();


};

#endif 
