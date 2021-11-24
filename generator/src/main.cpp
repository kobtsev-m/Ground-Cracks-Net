#include "indicator.h"
#include "images.h"

int main() {
    // Генерация изображений трещин
    cmplx ***indicator = generate_indicator(get_configs());
    generate_images(get_configs(), indicator);
}
