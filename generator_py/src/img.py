import numpy as np
import pandas as pd


def calculate_impulse(nu0, w):
    nu = 0.5 * w / np.pi
    g = 2.0 * nu*nu / (np.sqrt(np.pi) * nu0*nu0)
    return np.exp(-nu*nu / (nu0*nu0)) * g


def generate_images(configs, ind):

    # Параметры генерации изображения
    images_n = configs["images_n"]
    nx = configs["nx"]
    ny = configs["ny"]
    dx = configs["dx"]
    dy = configs["dy"]

    # Угол раскрытия
    beta = configs["opening_angle"]
    # Доминирующа частота
    nu0 = configs["dominant_frequency"]
    # Средняя скорость в референтной модели
    average_speed = configs["average_speed"]

    half_nx = int(nx / 2)
    half_ny = int(ny / 2)
    dpx = 2.0 * np.pi / (dx * (nx - 1))
    dpy = 2.0 * np.pi / (dy * (ny - 1))
    px = np.zeros(nx)
    py = np.zeros(ny)

    w = np.zeros((ny, nx), dtype=np.complex)
    fw = np.zeros((ny, nx), dtype=np.complex)
    cfw = np.zeros((ny, nx), dtype=np.complex)
    images = np.zeros((images_n, ny * nx))

    # Заполнение px и py
    for i in range(half_nx):
        px[i] = dpx * i
    if half_nx % 2 == 1:
        px[half_nx] = half_nx * dpx
    for i in range(half_nx):
        px[half_nx + i] = half_nx * dpx - i * dpx
    for i in range(half_ny):
        py[i] = dpy * i
    if half_ny % 2 == 1:
        py[half_ny] = half_ny * dpy
    for i in range(half_ny):
        py[half_ny + i] = half_ny * dpy - i * dpy

    # Подсчёт омега
    for ix in range(nx):
        for iy in range(ny):
            w[iy][ix] = np.sqrt(py[iy]*py[iy] + px[ix]*px[ix])
            w[iy][ix] *= (average_speed / (np.cos(beta) * 2.0))

    # Подсчёт импульса
    for iy in range(ny):
        for ix in range(nx):
            fw[iy][ix] = calculate_impulse(nu0, w[iy][ix])

    # Итерация по изображениям
    for i in range(images_n):
        # Вывод прогресса
        print(f'dataset: {i + 1}/{images_n}')

        # 2D преобразование фурье от DFN модели
        find = np.fft.fft2(ind[i])

        # Произведение фурье образа DFN модели и импульса
        for iy in range(ny):
            for ix in range(nx):
                cfw[iy][ix] = fw[iy][ix] * find[iy][ix]

        # Обратное 2D преобразование фурье
        cf = np.fft.ifft2(cfw)

        # Запись в файл (взятие целой части и модуля)
        for iy in range(ny):
            for ix in range(nx):
                images[i][iy*ny + ix] = abs(cf[iy][ix].real)

    # Сохранение изображений
    pd.DataFrame(images).to_csv(configs['images_file'], index=False, header=False)
