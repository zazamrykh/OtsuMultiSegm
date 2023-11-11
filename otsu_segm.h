#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IMAGEMATCHING_MATCHING_H
#define IMAGEMATCHING_MATCHING_H
#define HIST_SIZE 32
#define IMAGE_WIDTH 120
#define IMAGE_HEIGHT 120
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT)
#define MAX_COLOR 255
#define MAX_UINT8_VALUE 255
#define MAX_CLASS_NUMBER 32

typedef struct {
    int count;
    int arr[HIST_SIZE];
    int mean;
    int min_index, max_index;
    int min_color, max_color;
    int hist_step;
} Hist;

void otsu_segm(Hist *hist, int class_num, int *best_thresholds);

#endif //IMAGEMATCHING_MATCHING_H

#ifdef __cplusplus
}
#endif