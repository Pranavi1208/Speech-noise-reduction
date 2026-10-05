#include<stdio.h>
#include<stdlib.h>
#include "../include/noise_filter.h"

double avg_amplitude(short *window, size_t window_size)
{
    if (window_size == 0)
    {
        return 0.0;
    }

    double sum = 0.0;

    for (size_t i = 0; i < window_size; i++)
    {
        sum += abs(window[i]);
    }

    return sum / (double)window_size;

}

void detect_speech( short *samples, size_t number_of_samples, size_t window_size, int *speech_flags)
{
    if (window_size == 0 || samples == NULL || speech_flags == NULL)
    {
        return;
    }
    size_t number_of_windows = number_of_samples / window_size;
     
    double max_average_amplitude= 0.0;

    for( size_t i = 0; i < number_of_windows; i++)
    {
        short *window = samples + (i * window_size);
        double average = avg_amplitude(window, window_size);

        if (average > max_average_amplitude)
        {
            max_average_amplitude = average;
        }
    }
double threshold = SPEECH_THRESHOLD_FACTOR * max_average_amplitude;

// visit every window again 
 for ( size_t i = 0; i < number_of_windows; i++)
 {
    
    //get the current window
    short *window = samples + (i * window_size);
    // get the avg amplitude of each
    double average = avg_amplitude(window, window_size);
// compare it with treshhold 
    if (average >= threshold)
    {
        speech_flags[i] = 1; // speech
    }
    else
    {
        speech_flags[i] = 0; // not speech
 }

}
}



