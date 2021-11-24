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

void generate_images(nlohmann::json configs, cmplx ***indicator) {

    // Параметры генерации изображения
    int images_n = configs["images_n"];
    int nx = configs["nx"];
    int ny = configs["ny"];
    float dx = configs["dx"];
    float dy = configs["dy"];

    // Угол раскрытия
    float beta = configs["opening_angle"];
    // Доминирующа частота
    float nu0 = configs["dominant_frequency"];
    // Средняя скорость в референтной модели
    float average_speed = configs["average_speed"];

    int half_nx = floor(nx / 2);
    int half_ny = floor(ny / 2);
    float dpx = 2.0 * M_PI / (dx * (nx - 1));
    float dpy = 2.0 * M_PI / (dy * (ny - 1));
    auto *px = new cmplx[nx];
    auto *py = new cmplx[ny];

    auto **w = create_matrix(ny, nx);
    auto **fW = create_matrix(ny, nx);
    auto **fInd = create_matrix(ny, nx);
    auto **cFW = create_matrix(ny, nx);
    auto **cF = create_matrix(ny, nx);

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
    for (int ix = 0; ix < nx; ++ix) {
        for (int iy = 0; iy < ny; ++iy) {
            w[iy][ix] = sqrt(py[iy]*py[iy] + px[ix]*px[ix]);
            w[iy][ix] *= (average_speed / (cos(beta) * 2.0));
        }
    }

    // Подсчёт импульса в каждой точке
    for (int iy = 0; iy < ny; ++iy) {
        for (int ix = 0; ix < nx; ++ix) {
            fW[iy][ix] = calculate_impulse(nu0, w[iy][ix]);
        }
    }

    // Итерация по изображениям
    for (int i = 0; i < images_n; ++i) {
        // Вывод прогресса
        printf("dataset: %d/%d\n", i + 1, images_n);

        // 2D преобразование фурье от DFN модели
        simple_fft::FFT(indicator[i], fInd, ny, nx, error);

        // Произведение фурье образа DFN модели и импульса
        for (int iy = 0; iy < ny; ++iy) {
            for (int ix = 0; ix < nx; ++ix) {
                cFW[iy][ix] = fW[iy][ix] * fInd[iy][ix];
            }
        }

        // Обратное 2D преобразование фурье
        simple_fft::IFFT(cFW, cF, ny, nx, error);

        // Запись в файл (взятие целой части и модуля)
        for (int iy = 0; iy < ny; ++iy) {
            for (int ix = 0; ix < nx; ++ix) {
                bool isLast = ix == nx - 1 && iy == ny - 1;
                images_file << abs(cF[iy][ix].real()) << (isLast ? "" : ",");
            }
        }
        images_file << "\n";
    }

    // Закрытие файлa
    images_file.close();

    // Очистка всех массивов и матриц
    clear_matrix(cF, ny);
    clear_matrix(cFW, ny);
    clear_matrix(fInd, ny);
    clear_matrix(fW, ny);
    clear_matrix(w, ny);
    clear_matrix(indicator, images_n, ny);
    delete[] py;
    delete[] px;
}
