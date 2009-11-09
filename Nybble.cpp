#include "stdafx.h"

unsigned char de_nybblify(unsigned char ls_nybble, unsigned char ms_nybble)
{
	return ls_nybble + ( ms_nybble << 4 );
}

// 2-byte number
short little_endian_2_big_endian(short i)
{
    return ((i>>8)&0xff)+((i << 8)&0xff00);
}
