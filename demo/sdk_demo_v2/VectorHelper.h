#pragma once
#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES

sf::Vector2f Normalize(sf::Vector2f);
float Product(sf::Vector2f, sf::Vector2f);
float AngleTo(sf::Vector2f, sf::Vector2f);
float DegToRad(float);
float RadToDeg(float);