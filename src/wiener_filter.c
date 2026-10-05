#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/noise_filter.h"

/* double estimate_noise_power(short *samples, size_t number_of_samples, size_t noise_samples)
{
    double noise_sum = 0.0;

    if (noise_samples > number_of_samples)
    {
        noise_samples = number_of_samples;
    }
    
    for (size_t i = 0; i < noise_samples; i++)
    {
        noise_sum += (double)samples[i] * samples[i];
    }

    return noise_sum / noise_samples;
}
*/
double estimate_noise_power( short *samples, size_t number_of_samples, size_t window_size, int *speech_flags)
{
    if ( samples == NULL || speech_flags == NULL || window_size == 0)
    {
        return 0.0;
    }
size_t number_of_windows = number_of_samples / window_size;
    double noise_sum = 0.0;
    size_t noise_window_count = 0;

    for (size_t i = 0; i < number_of_windows; i++)
    {
        if (speech_flags[i] == 0) // not speech
        {
            short *window = samples + (i * window_size);
            
            for (size_t j = 0; j < window_size; j++)
            {
                noise_sum += (double)window[j] * window[j];
                
            }
            noise_window_count++;
        }
        
    }
     if ( noise_window_count == 0)
        {
            return 0.0;
        }
    return noise_sum / (noise_window_count * window_size);
}


double estimate_signal_power(short *samples, size_t number_of_samples, size_t index, size_t window_size)
{
    if( samples == NULL || window_size == 0)
    {
        return 0.0;
    }

    double signal_sum = 0.0;
    int half_window = window_size / 2;

    int start = (int)index - half_window;
    int end = (int) index + half_window;

    if (start < 0)
    {
        start = 0;
    }

    if ( end >= (int)number_of_samples)
    {
        end = (int)number_of_samples - 1;

    }

    for (int i = start; i <= end; i++)
    {
        signal_sum += (double)samples[i] * samples[i];
    }
    
    return signal_sum / (end - start + 1);
}

double calculate_gain(double signal_power, double noise_power)
{
    double gain = 0.0;
    if (signal_power + noise_power ==0)
    {
        return 0;
    }
    gain = signal_power / (signal_power + noise_power);
    if (gain < 0.0)
    { 
        gain = 0.0;
    }
    if (gain > 1.0)
    {
        gain = 1.0;
    }
    return gain;
}


/*void wiener_filter(short *samples, short *filtered_samples, size_t number_of_samples,  size_t noise_samples, size_t window_size)
{
    double noise_power;
    double signal_power;
    double gain;

    noise_power = estimate_noise_power(samples, number_of_samples, noise_samples);

    for (size_t i = 0; i < number_of_samples; i++)
    {
        signal_power = estimate_signal_power(samples, number_of_samples, i, window_size);
       gain = calculate_gain(signal_power, noise_power);
        filtered_samples[i] = (short)(gain * samples[i]);
    }
}*/

// improved 
void wiener_filter(short *samples, short *filtered_samples, size_t number_of_samples, size_t signal_window_size, size_t speech_window_size, int *speech_flags)
{
   if(samples == NULL || filtered_samples == NULL || speech_flags == NULL || signal_window_size == 0 || speech_window_size == 0)
   {
    return;
   }
   double noise_power;
   double signal_power;
   double gain;

   noise_power = estimate_noise_power(samples, number_of_samples, speech_window_size, speech_flags);
   printf("Noise Power = %lf\n", noise_power);


   for( size_t i = 0; i < number_of_samples; i++)
   {
    size_t window_index = i / speech_window_size;

    if ( speech_flags[window_index] ==1)
    {
        filtered_samples[i] = samples[i];
    }
    else
    { 
        signal_power = estimate_signal_power(samples, number_of_samples, i, signal_window_size);

        gain = calculate_gain(signal_power, noise_power);
        filtered_samples[i] = (short)(gain * samples[i]);
    }
    
   }

}





