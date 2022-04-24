#pragma once

#include "base.hpp"
#include <SFML/Graphics.hpp>

namespace drxl {

    sf::Color convert(const Color & color);
    Color convert(const sf::Color & color);
    sf::Vector3f cross(sf::Vector3f v1, sf::Vector3f v2);
    sf::Vector3f barycentric(int x0, int y0, int x1, int y1, int x2, int y2, int Px, int Py);

}
