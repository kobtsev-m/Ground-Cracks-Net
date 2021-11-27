import yaml

from ind import generate_indicator
from img import generate_images


def get_configs():
    with open('../configs/configs.yml') as file:
        return yaml.safe_load(file)


if __name__ == '__main__':
    configs = get_configs()
    ind = generate_indicator(configs)
    generate_images(configs, ind)
