#include "utils.h"

using namespace std;

// Функция создания 2-мерной матрицы
cmplx** create_matrix(int n, int m) {
    auto **matrix = new cmplx *[n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = new cmplx [m];
    }
    return matrix;
}

// Функция очистки 2-мерной матрицы
void clear_matrix(cmplx** matrix, int n) {
    for (int i = 0; i < n; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

// Функция создания 3-мерной матрицы
cmplx*** create_matrix(int n, int m, int p) {
    auto ***matrix = new cmplx **[n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = new cmplx *[m];
        for (int j = 0; j < m; ++j) {
            matrix[i][j] = new cmplx [p];
        }
    }
    return matrix;
}

// Функция очистки 3-мерной матрицы
void clear_matrix(cmplx*** matrix, int n, int m) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            delete[] matrix[i][j];
        }
        delete[] matrix[i];
    }
    delete[] matrix;
}

// Чтение конфигураций из файла
json get_configs() {
    ifstream config_file("../configs/configs.json");
    json configs;
    config_file >> configs;
    return configs;
}

// Генерация рандомного вещественного числа
float rand_float(float min, float max) {
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

// Генерация радномного целого числа
int rand_int(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

// Генерация рандомного значения - true или false
bool rand_bool() {
    return rand() % 2 == 0;
}

// Перевод угла в радианы
float to_radian(float angle) {
    return angle * M_PI / 180.0;
}
