/*
This project contains implementations of three DSP filters:

1. Moving Average Filter
2. Improved Wiener Filter
3. Spectral Subtraction (currently enabled)

To test another filter:
- Uncomment the desired filter section.
- Comment out the currently enabled filter section.
- Recompile the project.

Only one filter should be enabled at a time.
==========================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include "../include/noise_filter.h"

int main()
{
    // reading the header.
    WAVHeader header;

    if (read_wav("input/audio2.wav", &header) == -1)
    {
        return 1;
    }

   /*testing whether the header has been read & getting important information-
    printf("RIFF: %.4s\n", header.riff);
    printf("WAVE: %.4s\n", header.wave);
    printf("Sample Rate: %d\n", header.sample_rate);
    printf("Channels: %d\n", header.num_channels);
    printf("Bits Per Sample: %d\n", header.bits_per_sample);
    printf("Data Size: %d\n", header.data_size); */

    // reading the samples. 
size_t number_of_samples = header.data_size/(header.bits_per_sample/8);
printf("Number of Samples: %zu\n", number_of_samples);
 
 short *samples;
 samples = malloc(number_of_samples * sizeof(short));
  
   if (samples == NULL)
 {
    printf("memory allocation failed.\n");
    return 1;
 }   
 FILE *fp;

fp = fopen("input/audio2.wav", "rb");
 if(fp == NULL)
 {
    printf("Unable to open file.\n");
    free(samples);
    return 1;
  }
  fseek(fp, sizeof(WAVHeader), SEEK_SET);
  if(fread(samples,sizeof(short),number_of_samples,fp)!= number_of_samples)
{
    printf("Error reading samples.\n");

    fclose(fp);
    free(samples);

    return 1;
}
   /* TO TEST IF THE SAMPLES HAVE BEEN READ CORRECTLY 
   printf("\nFirst 10 Samples:\n");
       for(int i = 0; i < 10; i++)
     {
      printf("%d\n", samples[i]);
     }  */

fclose(fp);

/*1. MOVING AVERAGE FILTER.

short *filtered_samples;
size_t window_size = 5; 

if(window_size < 3 || window_size % 2 == 0)
{
    printf("Invalid window size.\n");
    free(samples);
    return 1;
}
filtered_samples = malloc( number_of_samples *sizeof(short));

if (filtered_samples == NULL)
{
    printf("Memory allocation failed.\n");

    free(samples);
    return 1;
}
moving_average(samples, filtered_samples, number_of_samples, window_size);
 /* to test moving average-
 printf("Original\tFiltered\n");sss
 for(size_t i = 0; i < 20; i++)
 {
    printf("%d\t\t%d\n"
           samples[i],
           filtered_samples[i]);
 }

if (write_wav("output/moving_average2-windowsize5.wav", &header, filtered_samples, number_of_samples) == -1)
{
    free(samples);
    free(filtered_samples);
    return 1;
}
printf("Filtered audio saved successfully!\n");
 

 free(samples);
 free(filtered_samples);

return 0;
}*/ 

// 2. WEINER FILTER- 

/*short *wiener_samples;
wiener_samples = malloc(number_of_samples * sizeof(short));

if (wiener_samples == NULL)
{
    printf("Memory allocation failed.\n");

    free(samples);
    return 1;
}

size_t noise_samples = 500; 
size_t window_size = 11;

wiener_filter(samples, wiener_samples, number_of_samples, noise_samples, window_size);

if (write_wav("output/wiener_filter2-noise500-window11.wav", &header, wiener_samples,
              number_of_samples) == -1)
{
    free(samples);
    free(wiener_samples);
    return 1;
}

printf("Wiener filtered audio saved successfully!\n");

free(samples);
free(wiener_samples);

return 0;

} */

// 3. Improved WEINER FILTER-
/*
short *wiener_samples;
wiener_samples = malloc(number_of_samples * sizeof(short));

if (wiener_samples == NULL)
{
    printf("Memory allocation failed.\n");

    free(samples);
    return 1;
}

size_t speech_window_size = 256;
size_t signal_window_size = 11;

size_t number_of_windows = number_of_samples / speech_window_size;
int *speech_flags = malloc(number_of_windows *sizeof(int));
if (speech_flags == NULL)
{
    printf("Memory allocation failed.\n");
    free(samples);
    free(wiener_samples);
    return 1;
}

detect_speech(samples, number_of_samples, speech_window_size, speech_flags);

for ( size_t i = 0; i < number_of_windows; i++)
{
    printf("Window %zu: %s\n", i, speech_flags[i] ? "Speech" : "Not Speech");
}

wiener_filter(samples, wiener_samples, number_of_samples, signal_window_size, speech_window_size, speech_flags);

if (write_wav("output/wiener_filter2-noise500-window11.wav", &header, wiener_samples,
              number_of_samples) == -1)
{
    free(samples);
    free(wiener_samples);
    return 1;
}

printf("Wiener filtered audio saved successfully!\n");


free(samples);
free(speech_flags);
free(wiener_samples);

return 0;
}
*/



// 3. SPECTRAL SUBTRACTION-

short *spectral_samples;

spectral_samples = malloc(number_of_samples * sizeof(short));


if( spectral_samples == NULL)
{
    printf("Memory allocation failed.\n");
    free(samples);
    return 1;
}
size_t frame_size = 256;

size_t number_of_frames = number_of_samples / frame_size; 

int *speech_flags = malloc(number_of_frames * sizeof(int));
if (speech_flags == NULL)
{
    printf("Memory allocation failed.\n");
    free(samples);
    free(spectral_samples);
    return 1;
}

detect_speech(samples, number_of_samples, frame_size, speech_flags);

double *noise_spectrum = malloc((frame_size / 2 + 1) * sizeof(double));
if(noise_spectrum == NULL)
{
    printf("Memory allocation failed.\n");
    free(samples);
    free(spectral_samples);
    free(speech_flags);
    return 1;
}

estimate_noise_spectrum(samples, noise_spectrum, number_of_samples, frame_size, speech_flags);

spectral_subtraction(samples, spectral_samples, number_of_samples, frame_size, noise_spectrum, speech_flags);

if (write_wav("output/spectral_subtraction2-frame256.wav", &header, spectral_samples, number_of_samples) == -1)
{
    free(samples);
    free(spectral_samples);
    free(speech_flags);
    free(noise_spectrum);
    return 1;
}
printf("Spectral Subtraction filtered audio saved successfully!\n");

free(samples);
free(spectral_samples);
free(speech_flags);
free(noise_spectrum);
return 0;
}










