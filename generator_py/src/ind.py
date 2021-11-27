import numpy as np
import numpy.random as npr
import pandas as pd


def is_in_rectangle(pi, pc, depth, length, a1, a2):
    dist1 = abs(a1*pi[0] - pi[1] + (pc[1] - a1*pc[0])) / np.sqrt(a1*a1 + 1)
    dist2 = abs(a2*pi[0] - pi[1] + (pc[1] - a2*pc[0])) / np.sqrt(a2*a2 + 1)
    return dist1 <= depth/2 and dist2 <= length/2


def relative_square(pi, pc, depth, length, a1, a2, dx, dy):
    all_points = 10
    interior_points = 0

    for i in range(all_points):
        pr = [
            npr.uniform(pi[0], pi[0] + dx),
            npr.uniform(pi[1], pi[1] + dy)
        ]
        if is_in_rectangle(pr, pc, depth, length, a1, a2):
            interior_points += 1

    return interior_points / all_points


def generate_indicator(configs):

    # Количество изображений
    images_n = configs["images_n"]

    # Рандомно ли генерируется угол
    is_random = configs["is_random"]

    # Параметры генерации изображения
    cracks_n = configs["cracks_n"]
    nx = configs["nx"]
    ny = configs["ny"]
    x0 = configs["x0"]
    y0 = configs["y0"]
    dx = configs["dx"]
    dy = configs["dy"]

    # Задание среднего значения и дисперсии для длины и ширины
    length_mean = configs["length_mean"]
    length_dispersion = configs["length_dispersion"]
    depth_mean = configs["depth_mean"]
    depth_dispersion = configs["depth_dispersion"]

    # Значения для угла поворота
    alpha_min = configs["alpha_min"]
    alpha_max = configs["alpha_max"]
    alpha_dispersion = configs["alpha_dispersion"]

    # Значения индикаторной функции
    indicator = np.zeros((images_n, ny, nx), dtype=np.float32)
    labels = np.zeros((images_n, 1), dtype=int)

    # Итерация по изображениям
    for i in range(images_n):

        # Вывод прогресса
        print(f'indicator: {i+1}/{images_n}')

        # Текщий угол
        alpha_mean = npr.randint(alpha_min, alpha_max) if is_random else i

        # Итерация по трещинам
        for j in range(cracks_n):
            # Равномерное распределние для координат центра
            pc = [
                npr.uniform(x0 + length_mean, x0 + nx*dx - length_mean),
                npr.uniform(y0 + length_mean, y0 + ny*dy - length_mean)
            ]
            # Генерация значения длины и ширины через распределение Гаусса
            length = npr.normal(length_mean, length_dispersion)
            depth = npr.normal(depth_mean, depth_dispersion)
            # Генерация угла через распределение Гаусса (и перевод в радианы)
            alpha = npr.normal(np.radians(alpha_mean), np.radians(alpha_dispersion))
            # Коэффициенты прямых
            tan_alpha = np.tan(alpha)
            a1 = -1 / (tan_alpha if tan_alpha else 10e-6)
            a2 = tan_alpha
            # Заполнение индикаторной функции
            for iy in range(ny):
                for ix in range(nx):
                    pi = [x0 + ix*dx, y0 + iy*dy]
                    indicator[i][iy][ix] += relative_square(pi, pc, depth, length, a1, a2, dx, dy)

        labels[i][0] = alpha_mean

    # Сохранение индикаторной функции
    pd.DataFrame(labels).to_csv(configs['labels_file'], index=False, header=False)

    return indicator
