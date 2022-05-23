#include "images.h"

using namespace std;

// Переменная для сохранения ошибки
const char *error;

// Подсчёт импульса по угловой скорости и nu0
cmplx calculate_impulse(float nu0, cmplx w) {
    cmplx nu = 0.5 * w / M_PI;
    cmplx g = 2.0 * nu*nu / (sqrt(M_PI) * nu0*nu0);
    return exp(-nu*nu / (double)(nu0*nu0)) * g;
}

void generate_images(json configs, cmplx ***indicator) {

    // Параметры генерации изображения
    int images_n = configs["images_n"];
    int nx = configs["nx"];
    int ny = configs["ny"];
    int dx = configs["dx"];
    int dy = configs["dy"];

    // Угол раскрытия
    float beta = configs["opening_angle"];
    // Доминирующа частота
    float nu0 = configs["dominant_frequency"];
    // Средняя скорость в референтной модели
    float average_speed = configs["average_speed"];

    // Зачения шагов для n, length и alpha + Их общее количество
    string n_generating = configs["n_generating"];
    float n_min = configs["n_min"];
    float n_max = configs["n_max"];
    float n_step = configs["n_step"];
    int n_count = (n_max - n_min) / n_step + 1;
    float length_min = configs["length_min"];
    float length_max = configs["length_max"];
    float length_step = configs["length_step"];
    int length_count = (length_max - length_min) / length_step + 1;
    int alpha_min = configs["alpha_min"];
    int alpha_max = configs["alpha_max"];
    float alpha_step = configs["alpha_step"];
    int alpha_count = (alpha_max - alpha_min) / alpha_step + 1;

    int images_base;
    if (n_generating == "complex_step") {
        images_base = n_count * length_count * alpha_count;
    } else {
        images_base = images_n;
    }

    int half_nx = floor(nx / 2);
    int half_ny = floor(ny / 2);
    float dpx = 2.0 * M_PI / (dx * (nx - 1));
    float dpy = 2.0 * M_PI / (dy * (ny - 1));
    auto *px = new cmplx[nx];
    auto *py = new cmplx[ny];

    auto **w = create_matrix(ny, nx);
    auto **fw = create_matrix(ny, nx);
    auto **find = create_matrix(ny, nx);
    auto **cfw = create_matrix(ny, nx);
    auto **cf = create_matrix(ny, nx);

    // Открытие файлa для изображений
    ofstream images_file;
    images_file.open(configs["images_file"]);

    // Заполнение px и py
    for (int i = 0; i < half_nx; ++i) {
        px[i] = dpx * i;
    }
    if (half_nx % 2 == 1) {
        px[half_nx] = half_nx * dpx;
    }
    for (int i = 0; i < half_nx; ++i) {
        px[half_nx + i] = half_nx * dpx - i * dpx;
    }
    for (int i = 0; i < half_ny; ++i) {
        py[i] = dpy * i;
    }
    if (half_ny % 2 == 1) {
        py[half_ny] = half_ny * dpy;
    }
    for (int i = 0; i < half_ny; ++i) {
        py[half_ny + i] = half_ny * dpy - i * dpy;
    }

    // Подсчёт омега для каждого столбца индикаторной функции
    // И подсчёт импульса в каждой точке
    for (int ix = 0; ix < nx; ++ix) {
        for (int iy = 0; iy < ny; ++iy) {
            w[iy][ix] = sqrt(py[iy]*py[iy] + px[ix]*px[ix]);
            w[iy][ix] *= (average_speed / (cos(beta) * 2.0));
            fw[iy][ix] = calculate_impulse(nu0, w[iy][ix]);
        }
    }

    // Итерация по изображениям
    for (int i = 0; i < images_base; i += 1) {

        // Вывод прогресса
        printf("images: %d/%d\n", i + 1, images_base);

        // 2D преобразование фурье от DFN модели
        simple_fft::FFT(indicator[i], find, ny, nx, error);

        // Произведение фурье образа DFN модели и импульса
        for (int iy = 0; iy < ny; ++iy) {
            for (int ix = 0; ix < nx; ++ix) {
                cfw[iy][ix] = fw[iy][ix] * find[iy][ix];
            }
        }

        // Обратное 2D преобразование фурье
        simple_fft::IFFT(cfw, cf, ny, nx, error);

        // Запись в файл (взятие целой части и модуля)
        for (int iy = 0; iy < ny; ++iy) {
            //for (int jy = 0; jy < dy; ++jy) {
                for (int ix = 0; ix < nx; ++ix) {
                    //for (int jx = 0; jx < dx; ++jx) {
                        bool is_last = (ix == nx - 1) && (iy == ny - 1);
                        images_file << cf[iy][ix].real() << (is_last ? "" : ",");
                    }
                //}
            }
        //}
        images_file << "\n";
    }

    // Закрытие файлa
    images_file.close();

    // Очистка всех массивов и матриц
    clear_matrix(cf, ny);
    clear_matrix(cfw, ny);
    clear_matrix(find, ny);
    clear_matrix(fw, ny);
    clear_matrix(w, ny);
    clear_matrix(indicator, images_n, ny);
    delete[] py;
    delete[] px;
}
