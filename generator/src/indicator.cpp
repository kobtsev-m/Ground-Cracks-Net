#include "indicator.h"

using namespace std;

bool is_in_rectangle(
    point pi, point pc,
    float depth, float length,
    float a1, float a2
) {
    float dist1 = abs(a1*pi.x - pi.y + (pc.y - a1*pc.x)) / sqrt(a1*a1 + 1);
    float dist2 = abs(a2*pi.x - pi.y + (pc.y - a2*pc.x)) / sqrt(a2*a2 + 1);
    return dist1 <= depth/2 && dist2 <= length/2;
}

float relative_square(
    point pi, point pc,
    float depth, float length,
    float a1, float a2,
    float dx, float dy
) {
    int all_points = 10;
    int interior_points = 0;

    for (int i = 0; i < all_points; ++i) {
        point pr = {
            rand_float(pi.x, pi.x + dx),
            rand_float(pi.y, pi.y + dy)
        };
        if (is_in_rectangle(pr, pc, depth, length, a1, a2)) {
            interior_points += 1;
        }
    }

    return (float)interior_points / (float)all_points;
}

float rand_gauss(float m, float d) {
    // fx - значение плотности распределения
    // diff - расстояние между средним значением (m) и x
    float diff = 0;
    if (d != 0) {
        float fx = rand_float(10e-6, 1 / sqrt(2 * M_PI * d*d));
        diff = sqrt(2 * d*d * log(1 / (fx * sqrt(2 * M_PI * d*d))));
    }
    return rand_bool() ? m + diff : m - diff;
}

float rand_uniform(float min_coord, float max_coord, float average_length) {
    return rand_float(min_coord + average_length, max_coord - average_length);
}

cmplx*** generate_indicator(json configs) {

    // Количество изображений
    int images_n = configs["images_n"];

    // Рандомно ли генерируется угол
    bool is_random = configs["is_random"];

    // Параметры генерации изображения
    int cracks_n = configs["cracks_n"];
    int nx = configs["nx"];
    int ny = configs["ny"];
    float x0 = configs["x0"];
    float y0 = configs["y0"];
    float dx = configs["dx"];
    float dy = configs["dy"];

    // Задание среднего значения и дисперсии для длины и ширины
    float length_mean = configs["length_mean"];
    float length_dispersion = configs["length_dispersion"];
    float depth_mean = configs["depth_mean"];
    float depth_dispersion = configs["depth_dispersion"];

    // Значения для угла поворота
    int alpha_min = configs["alpha_min"];
    int alpha_max = configs["alpha_max"];
    float alpha_dispersion = configs["alpha_dispersion"];

    // Массив, где хранятся значения индикаторной функции
    auto ***indicator = create_matrix(images_n, ny, nx);

    // Открытие файлов: для индикаторной функции и для углов
    ofstream labels_file;
    labels_file.open(configs["labels_file"]);

    // Итерация по изображениям
    for (int i = 0; i < images_n; ++i) {

        // Вывод прогресса
        printf("indicator: %d/%d\n", i + 1, images_n);

        // Текщий угол
        int alpha_mean = is_random ? rand_int(alpha_min, alpha_max) : i;

        // Заполнение нулями массива с индикаторной функцией
        for (int iy = 0; iy < ny; ++iy) {
            for (int ix = 0; ix < nx; ++ix) {
                indicator[i][iy][ix] = 0;
            }
        }

        // Итерация по трещинам
        for (int j = 0; j < cracks_n; ++j) {
            // Равномерное распределние для координат центра
            point pc = {
                rand_uniform(x0, x0 + nx*dx, length_mean),
                rand_uniform(y0, y0 + ny*dy, length_mean)
            };
            // Генерация значения длины и ширины через распределение Гаусса
            float length = rand_gauss(length_mean, length_dispersion);
            float depth = rand_gauss(depth_mean, depth_dispersion);
            // Генерация угла через распределение Гаусса (и перевод в радианы)
            float alpha = rand_gauss(
                to_radian(alpha_mean),
                to_radian(alpha_dispersion)
            );
            // Коэффициенты прямых
            float a1 = -1 / (tan(alpha) ? tan(alpha) : 10e-6);
            float a2 = tan(alpha);
            // Заполнение индикаторной функции
            for (int iy = 0; iy < ny; ++iy) {
                for (int ix = 0; ix < nx; ++ix) {
                    point pi = {x0 + ix*dx, y0 + iy*dy};
                    indicator[i][iy][ix] += relative_square(pi, pc, depth, length, a1, a2, dx, dy);
                }
            }
        }

        // Запись результатов в файл
        labels_file << alpha_mean << "\n";
    }

    // Закрытие файла
    labels_file.close();

    return indicator;
}
