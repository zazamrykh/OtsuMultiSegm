#include "otsu_segm.h"

void calculate_sigma(Hist *hist, int class_num, int *class_size, int *color_sums, float *best_sigma,
                      const int *thresholds, int *best_thresholds);

// Recursive method for iterating through hist
void run(Hist *hist, int class_num, int *thresholds, int *best_thresholds, int class_size[MAX_CLASS_NUMBER],
         int color_sums[MAX_CLASS_NUMBER], float *best_sigma, int current_class, int start_pos);

void otsu_segm(Hist *hist, int class_num, int *best_thresholds) {
    int class_size[MAX_CLASS_NUMBER];
    int color_sums[MAX_CLASS_NUMBER];
    int thresholds[MAX_CLASS_NUMBER];
    float best_sigma = 0;
    run(hist, class_num, thresholds, best_thresholds, class_size, color_sums, &best_sigma, 0, 0);
    for (int i = 0; i < class_num - 1; ++i) {
        best_thresholds[i] = hist->min_color + best_thresholds[i] * hist->hist_step;
    }
    best_thresholds[class_num - 1] = MAX_COLOR;
}

void run(Hist *hist, int class_num, int *thresholds, int *best_thresholds, int class_size[MAX_CLASS_NUMBER],
         int color_sums[MAX_CLASS_NUMBER], float *best_sigma, int current_class, int start_pos) {
    class_size[current_class] = 0;
    color_sums[current_class] = 0;
    for (int i = start_pos; i <= hist->max_index - (class_num - current_class + 2); ++i) {
        thresholds[current_class] = i;
        class_size[current_class] += hist->arr[i];
        color_sums[current_class] += (hist->min_color + i * hist->hist_step + hist->hist_step / 2) * hist->arr[i];
        if (current_class == class_num - 2) {
            calculate_sigma(hist, class_num, class_size, color_sums, best_sigma,
                            thresholds, best_thresholds);
        } else {
            run(hist, class_num, thresholds, best_thresholds, class_size, color_sums, best_sigma,
                current_class + 1, i + 1);
        }
    }
}

void calculate_sigma(Hist *hist, int class_num, int *class_size, int *color_sums, float *best_sigma,
                      const int *thresholds, int *best_thresholds) {
    // Need to calculate last class mean and last class proba
    class_size[class_num - 1] = hist->count;
    for (int i = 0; i < class_num - 1; ++i) {
        if (class_size[i] == 0) {
            return;
        }
        class_size[class_num - 1] -= class_size[i];
    }
    if (class_size[class_num - 1] == 0) {
        return;
    }

    color_sums[class_num - 1] = hist->count * hist->mean;
    for (int i = 0; i < class_num - 1; ++i) {
        color_sums[class_num - 1] -= color_sums[i];
    }

    float sigma = 0;
    for (int i = 0; i < class_num; ++i) {
        sigma += ((float) class_size[i] / (float) hist->count)
                * ((float) color_sums[i] / (float) class_size[i] - (float) hist->mean)
                * ((float) color_sums[i] / (float) class_size[i] - (float) hist->mean);
    }
    if (sigma > *best_sigma) {
        *best_sigma = sigma;
        for (int i = 0; i < class_num; ++i) {
            best_thresholds[i] = thresholds[i];
        }
    }
}

