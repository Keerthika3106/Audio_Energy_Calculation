
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define ENERGY_THRESHOLD INT64_C(2000)

// LIF parameters - demonstration values
#define BETA 0.8
#define INPUT_WEIGHT 1.0
#define FIRING_THRESHOLD 2.0
#define RESET_VALUE 0.0


// LIF neuron structure
typedef struct
{
    double membrane_potential;
    int output_spike;
} LIFNeuron;


// 1. Calculate energy of one audio frame
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


// 2. Calculate Zero-Crossing Rate
double calculate_zero_crossing_rate(const int16_t *samples,
                                    size_t frame_size)
{
    if (samples == NULL || frame_size < 2)
    {
        return 0.0;
    }

    size_t zero_crossings = 0;

    for (size_t i = 1; i < frame_size; i++)
    {
        if ((samples[i] > 0 && samples[i - 1] < 0) ||
            (samples[i] < 0 && samples[i - 1] > 0))
        {
            zero_crossings++;
        }
    }

    double zcr =
        (double)zero_crossings / (2.0 * (frame_size - 1));

    return zcr;
}


// 3. Sound / silence classification using energy
int classify_sound(int64_t energy)
{
    return (energy > ENERGY_THRESHOLD) ? 1 : 0;
}


// 4. Threshold-crossing spike encoder
int threshold_crossing_encode(int64_t previous_energy,
                              int64_t current_energy,
                              int64_t threshold)
{
    if (previous_energy <= threshold &&
        current_energy > threshold)
    {
        return 1;
    }

    return 0;
}


// 5. Initialize LIF neuron
void lif_initialize(LIFNeuron *neuron)
{
    neuron->membrane_potential = 0.0;
    neuron->output_spike = 0;
}


// 6. LIF neuron processing
int lif_update(LIFNeuron *neuron, int input_spike)
{
    // Leakage and integration
    neuron->membrane_potential =
        BETA * neuron->membrane_potential +
        INPUT_WEIGHT * input_spike;

    // Firing condition
    if (neuron->membrane_potential >= FIRING_THRESHOLD)
    {
        neuron->output_spike = 1;

        printf("       >>> LIF FIRED! Potential before reset: %.2f\n",
               neuron->membrane_potential);

        // Reset after firing
        neuron->membrane_potential = RESET_VALUE;
    }
    else
    {
        neuron->output_spike = 0;
    }

    return neuron->output_spike;
}


// 7. Process audio frame by frame
void process_audio_frames(const int16_t *audio,
                          size_t total_samples,
                          size_t frame_size)
{
    if (audio == NULL || frame_size < 2)
    {
        printf("Invalid input!\n");
        return;
    }

    printf("\n============================================================\n");
    printf("FRAME SIZE: %zu samples\n", frame_size);
    printf("============================================================\n");

    size_t frame_number = 1;
    size_t start = 0;

    int64_t previous_energy = 0;

    // One LIF neuron for this frame-size experiment
    LIFNeuron neuron;
    lif_initialize(&neuron);

    size_t encoder_spike_count = 0;
    size_t lif_spike_count = 0;

    printf("\nFrame | Energy | ZCR    | Sound | Encoder | LIF Output\n");
    printf("------------------------------------------------------------\n");

    while (start + frame_size <= total_samples)
    {
        // Calculate frame energy
        int64_t energy =
            calculate_frame_energy(&audio[start], frame_size);

        // Calculate zero-crossing rate
        double zcr =
            calculate_zero_crossing_rate(&audio[start],
                                         frame_size);

        // Calculate mean-square energy
        double mean_square =
            (double)energy / frame_size;

        // Classify sound or silence
        int sound = classify_sound(energy);

        // Generate encoder spike
        int input_spike = threshold_crossing_encode(
            previous_energy,
            energy,
            ENERGY_THRESHOLD
        );

        // Pass encoder spike to LIF neuron
        int output_spike = lif_update(&neuron, input_spike);

        // Count spikes
        encoder_spike_count += input_spike;
        lif_spike_count += output_spike;

        // Display results
        printf("%5zu | %6lld | %.4f | %-5s | %7d | %10d\n",
               frame_number,
               (long long)energy,
               zcr,
               sound ? "SOUND" : "QUIET",
               input_spike,
               output_spike);

        printf("       Mean-square energy: %.2f\n", mean_square);
        printf("       Membrane potential: %.2f\n",
               neuron.membrane_potential);

        // Update previous energy
        previous_energy = energy;

        frame_number++;
        start += frame_size;
    }

    if (start < total_samples)
    {
        printf("\nRemaining samples not processed: %zu\n",
               total_samples - start);
    }

    // Spike summary
    printf("\n--------------- SPIKE SUMMARY ---------------\n");
    printf("Total frames processed : %zu\n", frame_number - 1);
    printf("Encoder spikes         : %zu\n", encoder_spike_count);
    printf("LIF output spikes      : %zu\n", lif_spike_count);
    printf("----------------------------------------------\n");
}


// 8. Main function
int main(void)
{
    size_t total_samples;

    printf("================================================\n");
    printf(" AUDIO ENERGY + ZCR + SPIKE ENCODING + LIF\n");
    printf("================================================\n");

    printf("\nEnter the number of audio samples: ");

    if (scanf("%zu", &total_samples) != 1 ||
        total_samples == 0)
    {
        printf("Invalid number of samples!\n");
        return 1;
    }

    if (total_samples > 10000)
    {
        printf("Maximum 10000 samples allowed!\n");
        return 1;
    }

    int16_t audio_samples[total_samples];

    printf("\nEnter %zu audio sample values:\n", total_samples);

    for (size_t i = 0; i < total_samples; i++)
    {
        int value;

        printf("Sample %zu: ", i + 1);

        if (scanf("%d", &value) != 1 ||
            value < -32768 || value > 32767)
        {
            printf("Invalid audio sample!\n");
            return 1;
        }

        audio_samples[i] = (int16_t)value;
    }

    printf("\n============== CONFIGURATION ==============\n");
    printf("Energy threshold : %lld\n",
           (long long)ENERGY_THRESHOLD);
    printf("Beta             : %.2f\n", BETA);
    printf("Input weight     : %.2f\n", INPUT_WEIGHT);
    printf("Firing threshold : %.2f\n", FIRING_THRESHOLD);
    printf("Reset value      : %.2f\n", RESET_VALUE);

    printf("\nTotal audio samples: %zu\n", total_samples);

    // Experiment 1: Frame size 4
    process_audio_frames(audio_samples, total_samples, 4);

    // Experiment 2: Frame size 8
    process_audio_frames(audio_samples, total_samples, 8);

    return 0;
}
