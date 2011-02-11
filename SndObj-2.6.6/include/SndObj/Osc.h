 
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


#ifndef _OSC_H
#define _OSC_H

#include "FastOsc.h"


class Osc : public FastOsc {

 protected:

  SndObj* m_inputfr;
  SndObj* m_inputamp;

 public:

  Osc();
  Osc(Table* table, float fr, float amp, SndObj* inputfr,
      SndObj* inputamp = 0, int vecsize=DEF_VECSIZE, float sr=DEF_SR);

  ~Osc();
 
  void SetFreq(SndObj* inputfr){ m_inputfr = inputfr; }
  void SetAmp(SndObj* inputamp) { m_inputamp = inputamp; }
  int Connect(char* mess, void* input);
  short DoProcess();


};

#endif
