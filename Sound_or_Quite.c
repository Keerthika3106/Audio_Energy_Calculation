
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define THRESHOLD 2000

// Calculate energy of one audio frame
int64_t calculate_frame_energy(const int16_t *samples,
                               size_t frame_size)
{
    int64_t energy = 0;

    for (size_t i = 0; i < frame_size; i++)
    {
        int64_t value = samples[i];
        energy += value * value;
    }

    return energy;
}

// Classify audio based on energy threshold
int classify_sound(int64_t energy)
{
    if (energy > THRESHOLD)
    {
        return 1;  // SOUND
    }
    else
    {
        return 0;  // QUIET
    }
}

// Process audio samples frame by frame
void process_audio_frames(const int16_t *audio,
                          size_t total_samples,
                          size_t frame_size)
{
    if (audio == NULL || frame_size == 0)
    {
        printf("Invalid input!\n");
        return;
    }

    printf("\n====================================\n");
    printf("FRAME SIZE: %zu samples\n", frame_size);
    printf("====================================\n");

    size_t frame_number = 1;
    size_t start = 0;

    while (start + frame_size <= total_samples)
    {
        int64_t energy =
            calculate_frame_energy(&audio[start], frame_size);

        double mean_square =
            (double)energy / frame_size;

        int result = classify_sound(energy);

        printf("\nFrame %zu\n", frame_number);
        printf("Energy: %lld\n", (long long)energy);
        printf("Mean-square energy: %.2f\n", mean_square);

        if (result == 1)
        {
            printf("Classification: SOUND\n");
        }
        else
        {
            printf("Classification: QUIET\n");
        }

        frame_number++;
        start += frame_size;
    }

    if (start < total_samples)
    {
        printf("\nRemaining samples not processed: %zu\n",
               total_samples - start);
    }

    printf("\nTotal complete frames: %zu\n", frame_number - 1);
}

int main(void)
{
    size_t total_samples;

    printf("====================================\n");
    printf("       AUDIO SOUND DETECTION\n");
    printf("====================================\n");

    printf("\nEnter the number of audio samples: ");

    if (scanf("%zu", &total_samples) != 1 || total_samples == 0)
    {
        printf("Invalid number of samples!\n");
        return 1;
    }

    // Limit input size for this experiment
    if (total_samples > 10000)
    {
        printf("Maximum 10000 samples allowed!\n");
        return 1;
    }

    int16_t audio_samples[total_samples];

    printf("\nEnter %zu audio sample values:\n", total_samples);

    for (size_t i = 0; i < total_samples; i++)
    {
        printf("Sample %zu: ", i + 1);

        int value;

        if (scanf("%d", &value) != 1 ||
            value < -32768 || value > 32767)
        {
            printf("Invalid audio sample!\n");
            return 1;
        }

        audio_samples[i] = (int16_t)value;
    }

    printf("\nThreshold: %d\n", THRESHOLD);
    printf("Total audio samples: %zu\n", total_samples);

    // Experiment with two frame sizes
    process_audio_frames(audio_samples, total_samples, 4);

    process_audio_frames(audio_samples, total_samples, 8);

    return 0;
}
