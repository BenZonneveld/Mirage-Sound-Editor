
inline void fwrite_le_short(unsigned int s, FILE *file);
inline void fwrite_le_word(unsigned long int w, FILE *file);
void bmp_out(FILE *bmpfile, double **image, int32_t y, int32_t x);