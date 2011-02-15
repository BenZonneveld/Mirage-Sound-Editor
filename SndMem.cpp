#include "stdafx.h"
#include "SndMem.h"

SndMem::SndMem(char unsigned * wavdata, long wavsize, short mode, short channels, short bits, SndObj** inputlist, int vecsize, float sr) : SndIO(channels,bits,inputlist,vecsize,sr){
	m_wavedata = wavdata;
	m_datasize = wavsize;
	m_channels = 1;
	
	if ( m_bits > 0){
		m_buffsize = ( m_bits/8)*m_samples;
		if (!(m_buffer = new char[m_buffsize])){
			m_error=11;
			return;
		}

		m_cp=(char *) m_buffer;
	}
	m_datapos = 0;
}

SndMem::~SndMem()
{
	delete[] m_buffer;
}

short SndMem::Read()
{
	short items = 0;
	short items2 = 0;
	if ( m_datapos < m_datasize )
	{
		for(m_vecpos=0 ; m_vecpos < m_samples ; m_vecpos += m_channels)
		{
			if ( m_datapos + m_vecpos < m_datasize )
			{
				items++;
				items2++;
				m_output[m_vecpos]= static_cast<float>(m_wavedata[m_vecpos+m_datapos]); //(float) m_cp[m_vecpos];
			} else {
				items2++;
				m_output[m_vecpos]= 0.f;
			}
		}
		m_datapos += items;
		return items2;
	}
	return 0;
}

short SndMem::Write()
{
	return 0;
}

char * SndMem::ErrorMessage()
{
	return 0;
}
