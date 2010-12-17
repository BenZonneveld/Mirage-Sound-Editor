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

#include "stdafx.h"
#include <math.h>
#include "dsp_types.h"
#include "dsp.h"

/*void win_return()
{
        #ifdef WIN32
        if (quiet==0)
        {
                printf("Press Return to quit\n");
                getchar();
        }
        #endif
}*/


/*
inline uint16_t fread_le_short(FILE *file)              // read from file a 16-bit integer in little endian
{
        uint8_t        byte_a, byte_b;

        fread(&byte_a, 1, 1, file);
        fread(&byte_b, 1, 1, file);

        return (uint16_t) (byte_b<<8) | byte_a;
}

inline uint32_t fread_le_word(FILE *file)               // read from file a 32-bit integer in little endian
{
        uint8_t        byte_a, byte_b, byte_c, byte_d;

        fread(&byte_a, 1, 1, file);
        fread(&byte_b, 1, 1, file);
        fread(&byte_c, 1, 1, file);
        fread(&byte_d, 1, 1, file);

        return (uint32_t) (byte_d<<24) | (byte_c<<16) | (byte_b<<8) | byte_a;
}

inline void fwrite_le_short(uint16_t s, FILE *file)     // write to file a 16-bit integer in little endian
{
        uint8_t byte;

        byte = s & 0xFF;
        fwrite(&byte, 1, 1, file);
        byte = (s>>8) & 0xFF;
        fwrite(&byte, 1, 1, file);
}

inline void fwrite_le_word(uint32_t w, FILE *file)      // write to file a 32-bit integer in little endian
{
        uint8_t byte;

        byte = w & 0xFF;
        fwrite(&byte, 1, 1, file);
        byte = (w>>8) & 0xFF;
        fwrite(&byte, 1, 1, file);
        byte = (w>>16) & 0xFF;
        fwrite(&byte, 1, 1, file);
        byte = (w>>24) & 0xFF;
        fwrite(&byte, 1, 1, file);
}

char *getstring()
{
        signed long len_str, i;
        char a[FILENAME_MAX], *b;

        fgets(a, sizeof(a), stdin);
        len_str=strlen(a);

        b=(char *)malloc(len_str * sizeof(char));

        for (i=0; i<len_str; i++)
                b[i]=a[i];

        b[len_str-1]=0;

        return b;
}

unsigned long int str_isnumber(char *string)      // returns 1 is string is a valid float number, 0 otherwise
{
        int32_t i;
        int32_t size = strlen(string);
        char c;

        if (size==0)
                return 0;

        c = string[0];
        if (! ((c>='0' && c<='9') || c=='+' || c=='-'))
                return 0;

        for (i=1; i<size; i++)
        {
                c = string[i];

                if (! ((c>='0' && c<='9') || c=='.' || c=='e'))
                        return 0;
        }

        return 1;
}
*/