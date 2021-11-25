#ifndef DATASET_GENERATOR_IMAGES_H
#define DATASET_GENERATOR_IMAGES_H

// Настройка для библиотеки simple_fft
#define __USE_SQUARE_BRACKETS_FOR_ELEMENT_ACCESS_OPERATOR

#include <iostream>
#include <cmath>
#include <simple_fft/fft.h>
#include <nlohmann/json.hpp>
#include "utils.h"

void generate_images(json, cmplx***);

#endif //DATASET_GENERATOR_IMAGES_H
