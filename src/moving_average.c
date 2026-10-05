#include "../include/noise_filter.h"

void moving_average(short *samples, short *filtered_samples, size_t number_of_samples, size_t window_size)
  {
    
    int half_window = window_size / 2;

    int j;

    int sum=0;

    if (window_size < 3 || window_size % 2 == 0)
  {
    printf("Invalid window size.\n");
    return;
  }

    for (size_t k = 0; k < half_window; k++)
{
    filtered_samples[k] = samples[k];
}

for (size_t k = number_of_samples - half_window;
     k < number_of_samples;
     k++)
{
    filtered_samples[k] = samples[k];
}
for (size_t i = (size_t)half_window;
     i < number_of_samples - (size_t)half_window;
     i++)
{
    sum = 0;

    for (j = -half_window;
         j <= half_window;
         j++)
    {
        int index = (int)i + j;
sum += samples[index];
    }

int average = sum / (int)window_size;
filtered_samples[i] = (short)average;
  }
}



  