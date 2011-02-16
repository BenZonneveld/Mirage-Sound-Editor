#include "stdafx.h"
#include "SndMem.h"

SndMem::SndMem(short * wavdata, long wavsize, short mode, short channels, short bits, SndObj** inputlist, int vecsize, float sr) : SndIO(channels,bits,inputlist,vecsize,sr){
	m_wavedata = wavdata;
	m_datasize = wavsize;
	m_channels = 1;
	
/*	if ( m_bits > 0){
		m_buffsize = ( m_bits/8)*m_samples;
		if (!(m_buffer = new char[m_buffsize])){
			m_error=11;
			return;
		}

		m_cp=(char *) m_buffer;
		m_sp =(short *)m_buffer;
	}*/
	m_datapos = 0;
}

SndMem::~SndMem()
{
//	delete[] m_buffer;
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
				m_output[m_vecpos]= static_cast<float>(m_wavedata[m_vecpos+m_datapos]); 
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
	int i,n;
	int items=0;
	for (m_vecpos=n=0; m_vecpos < m_samples ; m_vecpos += m_channels)
	{
		for (i=0; i< m_channels ; i++)
		{
			if (m_IOobjs[i])
			{
				m_wavedata[m_datapos+m_vecpos+i]=static_cast<short>(m_IOobjs[i]->Output(m_vecpos));
				items++;
			}
		}
	}
	m_datapos += items;
	return 0;
}

char * SndMem::ErrorMessage()
{
	return 0;
}
