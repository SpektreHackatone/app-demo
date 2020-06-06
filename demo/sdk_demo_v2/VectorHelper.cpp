#include "VectorHelper.h"
#include <math.h>

sf::Vector2f Normalize(sf::Vector2f vector)
{
	if (vector.x == 0 && vector.y == 0)
	{
		return vector;
	}
	return sf::Vector2f(
		vector.x / (std::sqrt(std::pow(vector.x, 2) + std::pow(vector.y, 2))),
		vector.y / (std::sqrt(std::pow(vector.x, 2) + std::pow(vector.y, 2)))
	);
}

float Product(sf::Vector2f left, sf::Vector2f right)
{
	return left.x * right.x + left.y * right.y;
}

float AngleTo(sf::Vector2f from, sf::Vector2f to)
{
	return atan2(from.y - to.y, from.x - to.x);
}

float DegToRad(float deg)
{
	return deg * M_PI / 180;
}

float RadToDeg(float rad)
{
	return rad * 180 / M_PI;
}