#include "../include/noise_filter.h"
#include "../include/kiss_fftr.h"
#include "../include/kiss_fft.h"
#include <math.h>

void estimate_noise_spectrum(short *samples, double *noise_spectrum, size_t number_of_samples, size_t frame_size, 
    int *speech_flags)

{
    for( size_t i = 0; i < frame_size/2+1; i++)
    {
        noise_spectrum[i] = 0.0;
    }

// frame buffer allocation
kiss_fft_scalar * frame_buffer = malloc(frame_size * sizeof(kiss_fft_scalar));
if (frame_buffer == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }
// freq buffer allocation
kiss_fft_cpx *freq_buffer = malloc((frame_size/2 + 1) * sizeof(kiss_fft_cpx));

   if (freq_buffer == NULL)
     {
        printf("Memory allocation failed.\n");
        free(frame_buffer);
        return;
     }
// creating the kiss_fftr_cfg object for the fft
kiss_fftr_cfg cfg = kiss_fftr_alloc(frame_size, 0, NULL, NULL);
if ( cfg == NULL)
    { 
        printf("FFT allocation failed.\n");
        free(frame_buffer);
        free(freq_buffer);
        return;
    }

size_t number_of_frames = number_of_samples / frame_size;
size_t noise_frame_count = 0;


    for ( size_t i = 0; i < number_of_frames; i++)
    {
        if (speech_flags[i] == 0) // not speech 
    {
            short *frame_pointer = samples + (i * frame_size);
            for ( size_t j = 0; j < frame_size; j++)
             {
                frame_buffer[j] = frame_pointer[j];
             }
        kiss_fftr(cfg, frame_buffer, freq_buffer);
        for ( size_t k = 0; k < frame_size/2+1; k++)
        {
            double magnitude = sqrt(freq_buffer[k].r * freq_buffer[k].r + freq_buffer[k].i * freq_buffer[k].i);
            noise_spectrum[k] += magnitude;
        }
        noise_frame_count++;
    }
    }
    if (noise_frame_count == 0)
    {
        free(frame_buffer);
        free(freq_buffer);
        free(cfg);
        return;
    }
for ( size_t i = 0; i < frame_size/2+1; i++)
    {
        noise_spectrum[i] /= noise_frame_count;
    }
free(frame_buffer);
free(freq_buffer);
free(cfg);  
return;
}

void spectral_subtraction(short *samples, short *filtered_samples, size_t number_of_samples, size_t frame_size, double *noise_spectrum, int *speech_flags)
{
    if (samples == NULL || filtered_samples == NULL ||noise_spectrum == NULL ||speech_flags == NULL ||frame_size == 0)
    {
        return;
    }

    // frame buffer allocation
    kiss_fft_scalar *frame_buffer = malloc(frame_size * sizeof(kiss_fft_scalar));
    if (frame_buffer == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    // output buffer allocation
    kiss_fft_scalar *output_buffer = malloc(frame_size * sizeof(kiss_fft_scalar));
    if (output_buffer == NULL)
    {
        printf("Memory allocation failed.\n");
        free(frame_buffer);
        return;
    }

    // Allocate frequency buffer
    kiss_fft_cpx *freq_buffer = malloc((frame_size / 2 + 1) * sizeof(kiss_fft_cpx));
    if (freq_buffer == NULL)
    {
        printf("Memory allocation failed.\n");
        free(frame_buffer);
        free(output_buffer);
        return;
    }
    
    kiss_fft_cpx *clean_freq_buffer = malloc((frame_size / 2 + 1 ) * sizeof(kiss_fft_cpx));
    if (clean_freq_buffer == NULL)
    {
        printf("Memory allocation failed.\n");
        free(frame_buffer);
        free(output_buffer);
        free(freq_buffer);
        free(clean_freq_buffer);
        return;
    }

    // FFT configuration
    kiss_fftr_cfg fft_cfg = kiss_fftr_alloc(frame_size, 0, NULL, NULL);

    // IFFT configuration
    kiss_fftr_cfg ifft_cfg = kiss_fftr_alloc(frame_size, 1, NULL, NULL);

    if (fft_cfg == NULL || ifft_cfg == NULL)
    {
        printf("FFT allocation failed.\n");
        free(frame_buffer);
        free(output_buffer);
        free(freq_buffer);

        if (fft_cfg != NULL)
            free(fft_cfg);

        if (ifft_cfg != NULL)
            free(ifft_cfg);

        return;
    }

    size_t number_of_frames = number_of_samples / frame_size;

    for (size_t i = 0; i < number_of_frames; i++)
    {
        if (speech_flags[i] == 1)
        {
            // Copy the entire speech frame unchanged
            for (size_t j = 0; j < frame_size; j++)
            {
                filtered_samples[i * frame_size + j] =
                    samples[i * frame_size + j];
            }
        }
        else
        {
            short *frame_pointer = samples + (i * frame_size);

            // Copy frame into FFT buffer
            for (size_t j = 0; j < frame_size; j++)
            {
                frame_buffer[j] = frame_pointer[j];
            }

            // Perform FFT
            kiss_fftr(fft_cfg, frame_buffer, freq_buffer);
        for( size_t k = 0; k < frame_size/ 2+1; k++)
        {
            double magnitude = sqrt(freq_buffer[k].r *freq_buffer[k].r +
            freq_buffer[k].i * freq_buffer[k].i);

            double phase = atan2(freq_buffer[k].i , freq_buffer[k].r);

            double clean_magnitude = magnitude - noise_spectrum[k];
            if ( clean_magnitude < 0)
            {
                clean_magnitude = 0;
            }
            clean_freq_buffer[k].r = clean_magnitude * cos(phase);
            clean_freq_buffer[k].i = clean_magnitude * sin(phase);

        }
        kiss_fftri(ifft_cfg, clean_freq_buffer, output_buffer);
        for(size_t j = 0; j< frame_size; j++)
        {
           filtered_samples[i * frame_size + j] =
    (short)(output_buffer[j] / frame_size);
    

        }
     }
    }
    
    free(frame_buffer);
    free(output_buffer);
    free(freq_buffer);
    free(clean_freq_buffer);
    
    free(fft_cfg);
    free(ifft_cfg);
}





