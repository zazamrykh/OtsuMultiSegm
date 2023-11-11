#include <opencv2/opencv.hpp>
#include <filesystem>
#include <vector>
#include <string>
#include "otsu_segm.h"

using namespace cv;

void display_image(const int *image, int width, int height, const String& title, bool wait_key);

void calculate_hist(const int image[IMAGE_SIZE], Hist *hist, int blur_radius);

void from_cv_to_array(Mat *opencv_image, int image[IMAGE_SIZE]);

void copy_image(int where_copy[IMAGE_SIZE], const int what_copy[IMAGE_SIZE]);

int get_mean(const int *image, int x, int y, int image_width, int image_height, int blur_radius);

std::vector<std::string> getFilesInDirectory(const std::string& directoryPath);

void print_hist(Hist* hist);

void mean_filter_hist(Hist *hist, int filter_radius);

int main() {
    Mat image_opencv = imread("..//examples//lenna.png", IMREAD_GRAYSCALE);
    int image[IMAGE_SIZE];
    from_cv_to_array(&image_opencv, image);
    display_image(image, IMAGE_WIDTH, IMAGE_HEIGHT, "input image", false);

    // Simple use case
    Hist hist;
    // Params of method
    int class_num = 5;
    int blur_radius = 1; // Blur to image
    int filter_radius = 1; // Filter hist
    calculate_hist(image, &hist, blur_radius);
    mean_filter_hist(&hist, filter_radius);

    int thresholds[MAX_CLASS_NUMBER];
    otsu_segm(&hist, class_num, thresholds);
    for (int i = 0; i < class_num - 1; ++i) {
        printf("Threshold %d, value %d\n", i, thresholds[i]);
    }
    // Visualise result
    for (int & i : image) {
        for (int j = 0; j < class_num; ++j) {
            if (i <= thresholds[j]) {
                i = MAX_COLOR / class_num * (j + 1);
                break;
            }
        }
    }
    display_image(image, IMAGE_WIDTH, IMAGE_HEIGHT, "Result", false);
    // Use case for some count of classes
//    int blur_radius = 1;
//    int filter_radius = 1;
//    int thresholds[MAX_CLASS_NUMBER];
//
//    Hist hist;
//    calculate_hist(image, &hist, blur_radius);
//    mean_filter_hist(&hist, filter_radius);
//    int class_range = 10;
//    int initial_image[IMAGE_SIZE];
//    copy_image(initial_image, image);
//    for (int class_num = 2; class_num < class_range; ++class_num) {
//        otsu_segm(&hist, class_num, thresholds);
//        for (int i = 0; i < IMAGE_SIZE; ++i) {
//            for (int j = 0; j < class_num; ++j) {
//                if (initial_image[i] <= thresholds[j]) {
//                    image[i] = MAX_COLOR / class_num * (j + 1);
//                    break;
//                }
//            }
//        }
//
//        display_image(image, IMAGE_WIDTH, IMAGE_HEIGHT, std::to_string(class_num), false);
//    }

    waitKey(0);
    return 0;
}

void mean_filter_hist(Hist *hist, int filter_radius) {
    int buffer_hist[HIST_SIZE];
    for (int i = 0; i < HIST_SIZE; ++i) {
        buffer_hist[i] = hist->arr[i];
    }

    for (int i = 0; i < HIST_SIZE; ++i) {
        int number_of_participants = 0;
        int sum = 0;
        for (int j = max(0, i - filter_radius); j <= min(HIST_SIZE - 1, i + filter_radius); ++j) {
            number_of_participants++;
            sum += buffer_hist[j];
        }
        hist->arr[i] = sum / number_of_participants;
    }
}

void print_hist(Hist* hist) {
    printf("Histogram:\n");
    for (unsigned short i : hist->arr) {
        printf("%d ", i);
    }
    printf("\n");
}

std::vector<std::string> getFilesInDirectory(const std::string& directoryPath) {
    std::vector<std::string> fileNames;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            fileNames.push_back(entry.path().filename().string());
        }
    }

    return fileNames;
}

void copy_image(int where_copy[IMAGE_SIZE], const int what_copy[IMAGE_SIZE]) {
    for (int i = 0; i < IMAGE_SIZE; ++i) {
        where_copy[i] = what_copy[i];
    }
}


void calculate_hist(const int image[IMAGE_SIZE], Hist* hist, int blur_radius) {
    hist->count = IMAGE_SIZE;
    int min_color = MAX_COLOR;
    int max_color = 0;
    for (int i = 0; i < IMAGE_SIZE; ++i) {
        if (image[i] > max_color) {
            max_color = image[i];
        }
        if (image[i] < min_color) {
            min_color = image[i];
        }
    }
    hist->min_color = min_color;
    hist->max_color = max_color;
    hist->hist_step = (max_color - min_color) / HIST_SIZE;
    for (int & i : hist->arr) {
        i = 0;
    }

    int sum = 0;
    hist->min_index = HIST_SIZE - 1;
    hist->max_index = 0;
    for (int y = 0; y < IMAGE_HEIGHT; ++y) {
        for (int x = 0; x < IMAGE_WIDTH; ++x) {
            int pixel_index = y * IMAGE_HEIGHT + x;
            sum += image[pixel_index];
            int hist_index = (get_mean(image, x, y, IMAGE_WIDTH, IMAGE_HEIGHT, blur_radius)
                    - hist->min_color) / hist->hist_step;

            if (hist_index >= HIST_SIZE) {
                hist_index--;
            }
            hist->arr[hist_index]++;

            if (hist_index > hist->max_index) {
                hist->max_index = hist_index;
            }
            if (hist_index < hist->min_index) {
                hist->min_index = hist_index;
            }
        }
    }
    hist->mean = sum / (IMAGE_SIZE);
}

int get_mean(const int *image, int x, int y, int image_width, int image_height, int blur_radius) {
    int number_of_participants = 0;
    int sum_of_color = 0;
    for (int y_iter = max(0, y - blur_radius); y_iter < min(image_height, y + blur_radius + 1); ++y_iter) {
        for (int x_iter = max(0, x - blur_radius); x_iter < min(image_width, x + blur_radius + 1); ++x_iter) {
            number_of_participants++;
            sum_of_color += image[y_iter * image_width + x_iter];
        }
    }
    return sum_of_color / number_of_participants;
}

void display_image(const int *image, int width, int height, const String& title, bool wait_key) {
    Mat mat_image((int) width, (int) height, CV_8U);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            mat_image.at<uint8_t>(y, x) = (uint8_t ) (((int) image[y * width + x]) * MAX_UINT8_VALUE / MAX_COLOR);
        }
    }
    imshow(title, mat_image);
    if (wait_key) {
        waitKey(0);
    }
}

int cast_char_to(uchar value) {
    return (int) (((int) value) * MAX_COLOR / MAX_UINT8_VALUE);
}

void from_cv_to_array(Mat *opencv_image, int image[IMAGE_SIZE]) {
    for (int i = 0; i < opencv_image->rows * opencv_image->cols; ++i) {
        image[i] = cast_char_to(opencv_image->at<uchar>(i));
    }
}