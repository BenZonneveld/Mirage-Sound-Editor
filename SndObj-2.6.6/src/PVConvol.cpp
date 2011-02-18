 
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
// PVConvol.cpp: implementation of the PVConvol class.
//
//////////////////////////////////////////////////////////////////////

#include "PVConvol.h"


PVConvol::PVConvol(){
  m_hopsize = DEF_VECSIZE;
  m_framenos = 3;
  m_frame = new float*[m_framenos];
  for(int n=0; n < m_framenos; n++){
    m_frame[n] = new float[m_vecsize];
    m_frame[n][0] = m_frame[n][1] = 0.f;
    for(int i = 2; i < m_vecsize; i += 2){
      m_frame[n][i] = 0.f;
      m_frame[n][i+1] = (i/2)*m_sr/m_vecsize; 
    }
		
  }
  AddMsg("convol time", 31);
  m_cur = 0;
}
PVConvol::PVConvol(SndObj* input, int convolframes, int hopsize,
	       int vecsize, float sr)
  : SpecMult(input, 0, vecsize, sr){
	
  m_hopsize = hopsize;
  m_framenos = convolframes;
  m_frame = new float*[m_framenos];
  for(int n=0; n < m_framenos; n++){
    m_frame[n] = new float[m_vecsize];
    m_frame[n][0] = m_frame[n][1] = 0.f;
    for(int i = 2; i < m_vecsize; i += 2){
      m_frame[n][i] = 0.f;
      m_frame[n][i+1] = (i/2)*m_sr/m_vecsize; 
    }
		
  }
  AddMsg("convol time", 31);
  m_cur = 0;
}

PVConvol::~PVConvol(){
	
  delete[] m_frame;
	
}

void PVConvol::SetConvolTime(int convolframes){
  delete[] m_frame;
  m_framenos = convolframes;
  m_frame = new float*[m_framenos];
  for(int n=0; n < m_framenos; n++){
    m_frame[n] = new float[m_vecsize];
    m_frame[n][0] = m_frame[n][1] = 0.f;
    for(int i = 2; i < m_vecsize; i += 2){
      m_frame[n][i] = 0.f;
      m_frame[n][i+1] = (i/2)*m_sr/m_vecsize; 
    }
		
  }
  m_cur = 0;
}
int PVConvol::Set(char* mess, float value){
  switch(FindMsg(mess)){   
  case 31:
    SetConvolTime((int)value);
    return 1;
  default:
    return SpecMult::Set(mess,value);
  }
	
}


short
PVConvol::DoProcess(){
  if(!m_error){
    if(m_input){
      double sum, in;
      for(m_vecpos=0; m_vecpos < m_vecsize; m_vecpos++)
			{	
				if(m_enable) 
				{
				  m_frame[m_cur][m_vecpos] = m_input->Output(m_vecpos); 

					for(int n=0; n < m_framenos; n++)
					{ 
						in = m_frame[n][m_vecpos];
						sum += (in >= 0  ? in : -in);
					}
					m_output[m_vecpos] = sum/m_framenos; 
					sum = 0.f;
				}	else m_output[m_vecpos] = 0.f;
      }
      m_cur++;
      m_cur %= m_framenos;
			
      return 1;
    } else 
      m_error = 3;
    return 0;
  }
  return 0;
}