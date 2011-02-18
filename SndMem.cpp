#include "stdafx.h"
#include "SndMem.h"

SndMem::SndMem(short * wavdata, long wavsize, short mode, short channels, short bits, SndObj** inputlist, int vecsize, float sr) : SndIO(channels,bits,inputlist,vecsize,sr){
	m_swavedata = wavdata;
	m_datasize = wavsize;
	m_channels = 1;
	m_datapos = 0;
}

SndMem::SndMem(char unsigned * wavdata, long wavsize, short mode, short channels, short bits, SndObj** inputlist, int vecsize, float sr) : SndIO(channels,bits,inputlist,vecsize,sr){
	m_ucwavedata = wavdata;
	m_datasize = wavsize;
	m_channels = 1;
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
	char NewValue;
	if ( m_datapos < m_datasize )
	{
		for(m_vecpos=0 ; m_vecpos < m_samples ; m_vecpos += m_channels)
		{
			if ( m_datapos + m_vecpos < m_datasize )
			{
				items++;
				items2++;
				switch (m_bits)
				{
					case 16:
						m_output[m_vecpos]=static_cast<float>(m_swavedata[m_vecpos+m_datapos]);
						break;
					case 8:
						NewValue=m_ucwavedata[m_vecpos+m_datapos]-128;
						m_output[m_vecpos]=static_cast<float>(NewValue);
						break;
				}
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
	int i;
	int items=0;
	for (m_vecpos=0; m_vecpos < m_samples ; m_vecpos += m_channels)
	{
		if (m_IOobjs[0])
		{
			switch (m_bits)
			{
			case 16:
				m_swavedata[m_datapos+m_vecpos]=static_cast<short>(m_IOobjs[0]->Output(m_vecpos));
				break;
			case 8:
				m_ucwavedata[m_datapos+m_vecpos]=static_cast<char unsigned>(m_IOobjs[0]->Output(m_vecpos)+128);
//				m_ucwavedata[m_datapos+m_vecpos]=m_vecpos;
				break;
			}
		items++;
		}
	}
	m_datapos += items;
	return 0;
}

float SndMem::Output(int pos) 
{
	switch(m_bits){
		case 8:
			return m_ucwavedata[pos];
			break;
		case 16:
			return m_swavedata[pos];
			break;
	}
	return 0;
}

char * SndMem::ErrorMessage()
{
	return 0;
}
