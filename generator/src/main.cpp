#include "indicator.h"
#include "images.h"

int main() {
    // Для корректной работы рандома
    srand(time(nullptr));
    // Получение конфигурации
    json configs = get_configs();
    // Генерация индикаторной функции
    cmplx ***indicator = generate_indicator(configs);
    // Генерация изображений
    generate_images(configs, indicator);
}
