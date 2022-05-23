import numpy as np
import matplotlib.pyplot as plt


def skip_lines(file, n):
    for i in range(n):
        file.readline()


def main():
    target_i = 570
    
    with open('../dataset/21-05-22_1km-1km_train_images_5.csv') as f:
        skip_lines(f, target_i - 1)
        raw_image = f.readline()

    with open('../dataset/21-05-22_1km-1km_train_alpha_5.csv') as f:
        skip_lines(f, target_i - 1)
        raw_alpha = f.readline()

    with open('../dataset/21-05-22_1km-1km_train_length_5.csv') as f:
        skip_lines(f, target_i - 1)
        raw_length = f.readline()
        
    flat_image = list(map(float, raw_image.split(',')))
    image_side = int(np.sqrt(len(flat_image)))
    np_image = np.array(flat_image).reshape(image_side, image_side)
    alpha = float(raw_alpha)
    length = float(raw_length)

    plt.title(f'alpha: {alpha} | length: {length}')
    plt.imshow(np_image)
    plt.show()


main()
    
