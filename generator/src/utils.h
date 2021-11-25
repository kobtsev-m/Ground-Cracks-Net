#ifndef DATASET_GENERATOR_UTILS_H
#define DATASET_GENERATOR_UTILS_H

#include <fstream>
#include <random>
#include <complex>
#include <nlohmann/json.hpp>

// Для сокращённой запипси комплекного числа
typedef std::complex<double> cmplx;

// Для сокращённой запипси формата json
typedef nlohmann::json json;

// Для сокращённой записи координат
struct point {
    float x, y;
};

cmplx** create_matrix(int, int);
void clear_matrix(cmplx**, int);
cmplx*** create_matrix(int, int, int);
void clear_matrix(cmplx***, int, int);
json get_configs();
float rand_float(float, float);
int rand_int(int, int);
bool rand_bool();
float to_radian(float);

#endif //DATASET_GENERATOR_UTILS_H
