#include "stdafx.h"
#include "dsp_types.h"
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

void bmp_out(FILE *bmpfile, double **image, int32_t y, int32_t x)
{
        int32_t        i, iy, ix, ic;                  // various iterators
        int32_t        filesize, imagesize;
        uint8_t        zerobytes, val, zero=0;
        double vald;

        #ifdef DEBUG
        printf("bmp_out...\n");
        #endif

        zerobytes = 4 - ((x*3) & 3);           // computation of zero bytes
        if (zerobytes==4)
                zerobytes = 0;

        //********Tags********

        filesize = 56 + ((x*3)+zerobytes) * y;
        imagesize = 2 + ((x*3)+zerobytes) * y;

        fwrite_le_short(19778, bmpfile);
        fwrite_le_word(filesize, bmpfile);
        fwrite_le_word(0, bmpfile);
        fwrite_le_word(54, bmpfile);
        fwrite_le_word(40, bmpfile);
        fwrite_le_word(x, bmpfile);
        fwrite_le_word(y, bmpfile);
        fwrite_le_short(1, bmpfile);
        fwrite_le_word(24, bmpfile);
        fwrite_le_short(0, bmpfile);
        fwrite_le_word(imagesize, bmpfile);
        fwrite_le_word(2834, bmpfile);
        fwrite_le_word(2834, bmpfile);
        fwrite_le_word(0, bmpfile);
        fwrite_le_word(0, bmpfile);
        //--------Tags--------

        for (iy=y-1; iy!=-1; iy--)             // backwards writing
        {
                for (ix=0; ix<x; ix++)
                {
                        vald = image[iy][ix] * 255.0;

                        if (vald > 255.0)
                                vald = 255.0;

                        if (vald < 0.0)
                                vald = 0.0;

                        val = vald;

                        for (ic=2; ic!=-1; ic--)
                                fwrite(&val, 1, 1, bmpfile);
                }
                for (i=0; i<zerobytes; i++)
                        fwrite(&zero, 1, 1, bmpfile);        // write padding bytes
        }

        fwrite_le_short(0, bmpfile);

        fclose(bmpfile);

        #ifdef DEBUG
        printf("Image size : %dx%d\n", x, y);
        #endif
}
