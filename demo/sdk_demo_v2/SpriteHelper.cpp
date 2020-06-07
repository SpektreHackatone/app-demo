#include "SpriteHelper.h"

sf::Vector2f getScaling(sf::Vector2f prevScale, sf::Vector2f sizeFrom, sf::Vector2f sizeTo)
{
	sf::Vector2f result(sizeTo.x / sizeFrom.x, sizeTo.y / sizeFrom.y);
	if (sizeFrom.x == sizeTo.x)
	{
		result.x = prevScale.x;
	}
	if (sizeFrom.y == sizeTo.y)
	{
		result.y = prevScale.y;
	}
	return result;
}