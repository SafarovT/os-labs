#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

struct Line
{
    std::vector<sf::Vector2f> points;
    sf::Color color;
};