
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

int64_t calculate_frame_energy(const int16_t *samples,size_t frame_size){
    int64_t energy = 0;
    for (size_t i = 0; i < frame_size; i++){
        int64_t value = samples[i];
        energy += value * value;
    }
    return energy;
}

void process_audio_frames(const int16_t *audio,size_t total_samples,size_t frame_size){
    if (audio == NULL || frame_size == 0){
        printf("Invalid input!\n");
        return;
    }
    printf("FRAME SIZE: %zu samples\n", frame_size);
    size_t frame_number = 1;
    size_t start = 0;

    while (start + frame_size <= total_samples){
        int64_t energy =
            calculate_frame_energy(&audio[start], frame_size);
        double mean_square =
            (double)energy / frame_size;
        printf("\nFrame %zu\n", frame_number);
        printf("Starting sample index: %zu\n", start);
        printf("Energy: %lld\n", (long long)energy);
        printf("Mean-square energy: %.2f\n", mean_square);
        frame_number++;
        start += frame_size;
    }

    if (start < total_samples){
        printf("\nRemaining samples not processed: %zu\n",
               total_samples - start);
    }
    printf("\nTotal complete frames: %zu\n", frame_number - 1);
}

int main(void){
    int16_t audio_samples[] ={
        12, 22, 34, 31,
        34, 12, 3, -2,
        -12, -28, -10, 6,
        14, 20, 28, 15
    };

    size_t total_samples =
        sizeof(audio_samples) / sizeof(audio_samples[0]);
    printf("     AUDIO FRAME ENERGY ANALYSIS\n");
    printf("Total audio samples: %zu\n", total_samples);
    process_audio_frames(audio_samples, total_samples, 4);
    process_audio_frames(audio_samples, total_samples, 8);
    return 0;
}
