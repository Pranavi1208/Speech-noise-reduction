#include<stdio.h>
#include "../include/noise_filter.h"

// reading the header

int read_wav(const char *filename, WAVHeader *header)
{
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("Unable to open file:%s\n", filename);
        return -1;
    }

    if (fread(header, sizeof(WAVHeader), 1, fp) != 1)
    {
        printf("Error reading WAV header.\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

// writing the file 

int write_wav(const char *filename, WAVHeader *header, short *samples, size_t number_of_samples)
{

    FILE *fp;

    fp = fopen(filename, "wb");
if(fp == NULL)
 {
    printf("Unable to open file:%s\n", filename);
    return -1;
 }
if(fwrite(header, sizeof(WAVHeader), 1, fp) != 1)
{ 
  printf("Error writing the WAV header.\n ");
  fclose(fp);
  return -1;
}
if (fwrite(samples, sizeof(short), number_of_samples, fp) != number_of_samples)
 {
    printf("Error writing audio samples.\n");
    fclose(fp);
    return -1;
 }

   fclose(fp);
    return 0;
}

