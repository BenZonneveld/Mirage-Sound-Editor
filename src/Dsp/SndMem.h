#include <stdio.h>
#include <errno.h>
#include <SndObj/SndIO.h>

class SndMem : public SndIO{

protected:
	short m_mode;
	long	m_datapos;
	long	m_dataframes;
	short *	m_swavedata;
	char unsigned * m_ucwavedata;
	long	m_datasize;
	int		m_eof;

public:
	short GetMode() { return m_mode; }

	long GetDataFrames() { return m_dataframes; }
	SndMem(short * wavdata, long wavsize, short mode, short channels=1, short bits=8, SndObj** inputlist=0, int vecsize=DEF_VECSIZE, float sr=DEF_SR);
	SndMem(char unsigned * wavdata, long wavsize, short mode, short channels=1, short bits=8, SndObj** inputlist=0, int vecsize=DEF_VECSIZE, float sr=DEF_SR);
	~SndMem();
	short Read();
	short Write();
	int Eof() { return(m_datapos >= m_datasize); }
	char * ErrorMessage();
	float Output(int pos);
	float Output(int pos, int channel) {
		Output(pos); }
};