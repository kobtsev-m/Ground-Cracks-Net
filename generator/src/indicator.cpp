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

float rand_uniform(float min_coord, float max_coord) {
    return rand_float(min_coord, max_coord);
}

cmplx*** generate_indicator(json configs) {

    // Количество изображений
    int images_n = configs["images_n"];

    // Параметры генерации изображения
    int nx = configs["nx"];
    int ny = configs["ny"];
    float x0 = configs["x0"];
    float y0 = configs["y0"];
    int dx = configs["dx"];
    int dy = configs["dy"];

    // Значения для ширины
    float depth_mean = configs["depth_mean"];
    float depth_dispersion = configs["depth_dispersion"];

    // Значения для количества трещин
    string n_generating = configs["n_generating"];
    float n_min = configs["n_min"];
    float n_max = configs["n_max"];
    float n_step = configs["n_step"];
    float n_mean = configs["n_mean"];
    float n_dispersion = configs["n_dispersion"];
    int n_count = (n_max - n_min) / n_step + 1;

    // Значения для длины
    string length_generating = configs["length_generating"];
    float length_min = configs["length_min"];
    float length_max = configs["length_max"];
    float length_step = configs["length_step"];
    float length_mean = configs["length_mean"];
    float length_dispersion = configs["length_dispersion"];
    int length_count = (length_max - length_min) / length_step + 1;

    // Значения для угла поворота
    string alpha_generating = configs["alpha_generating"];
    int alpha_min = configs["alpha_min"];
    int alpha_max = configs["alpha_max"];
    float alpha_step = configs["alpha_step"];
    float alpha_mean = configs["alpha_mean"];
    float alpha_dispersion = configs["alpha_dispersion"];
    int alpha_count = (alpha_max - alpha_min) / alpha_step + 1;

    int images_base;
    if (n_generating == "complex_step") {
        images_base = n_count * length_count * alpha_count;
    } else {
        images_base = images_n;
    }

    // Массив, где хранятся значения индикаторной функции
    auto ***indicator = create_matrix(images_base, ny, nx);

    // Открытие файлов: для индикаторной функции и для лэйблов
    ofstream alpha_labels_file, length_labels_file;
    alpha_labels_file.open(configs["alpha_labels_file"]);
    length_labels_file.open(configs["length_labels_file"]);

    // Итерация по изображениям
    for (int i = 0; i < images_base; i += 1) {

        // Вывод прогресса
        printf("indicator: %d/%d\n", i + 1, images_base);

        int n_base;
        if (n_generating == "random") {
            n_base = rand_int(n_min, n_max);
        } else if (n_generating == "step") {
            n_base = n_min + i * n_step;
        } else if (n_generating == "fixed") {
            n_base = rand_gauss(n_mean, n_dispersion);
        } else if (n_generating == "complex_step") {
            int n_i = i % n_count;
            n_base = n_min + n_i * n_step;
        }

        float length_base;
        if (length_generating == "random") {
            length_base = rand_int(length_min, length_max);
        } else if (length_generating == "step") {
            length_base = length_min + i * length_step;
        } else if (length_generating == "fixed") {
            length_base = length_mean;
        } else if (length_generating == "complex_step") {
            int length_i = (i / n_count) % length_count;
            length_base = length_min + length_i * length_step;
        }

        float alpha_base;
        if (alpha_generating == "random") {
            alpha_base = rand_int(alpha_min, alpha_max);
        } else if (alpha_generating == "step") {
            alpha_base = alpha_min + i * alpha_step;
        } else if (alpha_generating == "fixed") {
            alpha_base = alpha_mean;
        } else if (length_generating == "complex_step") {
            int alpha_i = i / n_count / length_count;
            alpha_base = alpha_min + alpha_i * alpha_step;
        }

        // Заполнение нулями массива с индикаторной функцией
        for (int iy = 0; iy < ny; ++iy) {
            for (int ix = 0; ix < nx; ++ix) {
                indicator[i][iy][ix] = 0;
            }
        }

        // Итерация по трещинам
        for (int j = 0; j < n_base; ++j) {
            // Равномерное распределние для координат центра
            point pc = {
                rand_uniform(x0, x0 + nx*dx),
                rand_uniform(y0, y0 + ny*dy)
            };
            // Генерация значения ширины через распределение Гаусса
            float depth = rand_gauss(depth_mean, depth_dispersion);
            // Генерация значения длины через распределение Гаусса
            float length;
            if (length_generating == "fixed") {
                length = rand_gauss(length_mean, length_dispersion);
            } else {
                length = length_base;
            }
            // Генерация угла через распределение Гаусса (и перевод в радианы)
            float alpha;
            if (alpha_generating == "fixed") {
                alpha = rand_gauss(to_radian(alpha_mean), to_radian(alpha_dispersion));
            } else {
                alpha = alpha_base;
            }
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
        alpha_labels_file << (int) round(alpha_base) % 180 << "\n";
        length_labels_file << (int) round(length_base) << "\n";
    }

    // Закрытие файлов
    alpha_labels_file.close();
    length_labels_file.close();

    return indicator;
}
