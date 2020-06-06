#include "AnimatedSprite.h"
#include "VectorHelper.h"
#include <math.h>

AnimatedSprite::AnimatedSprite(
	const Scene::WeakPtr& scene,
	std::string spriteFile,
	sf::IntRect spriteRect,
	std::string splashFile,
	sf::Vector2f scaleSplash,
	float baseRotation,
	float speed)
	:_baseRotation(baseRotation),
	_current(0),
	_speed(speed),
	m_scene(scene),
	_splashFile(splashFile),
	_scaleSplash(scaleSplash),
	_rect(spriteRect)
{
	_texture.loadFromFile(spriteFile);
	_sprite.setTexture(_texture);
	_meta = getSpriteMetaData(*_sprite.getTexture(), _rect.width, _rect.height);
	_sprite.setOrigin(sf::Vector2f(_rect.width * _sprite.getScale().x / 2, _rect.height * _sprite.getScale().y / 2));
}

AnimatedSprite::~AnimatedSprite()
{
}

void AnimatedSprite::nextSprite()
{
	_sprite.setTextureRect(_rect);
	_current = (_current + 1) % _meta.totalNum;
	_rect.left = (_current % _meta.colNum) * _rect.width;
	_rect.top = (_current / _meta.colNum) * _rect.height;
}

int AnimatedSprite::getSumPixels(sf::Image img, sf::IntRect rect)
{
	int result = 0;
	for (size_t x = rect.left + 1; x < rect.left + rect.width - 1; x++)
	{
		for (size_t y = rect.top + 1; y < rect.top + rect.height - 1; y++)
		{
			sf::Color cPixel = img.getPixel(x, y);
			result += cPixel.a * (cPixel.r + cPixel.g + cPixel.b);
		}
	}
	return result;
}

SpriteMetaData AnimatedSprite::getSpriteMetaData(sf::Texture texture, int width, int height)
{
	SpriteMetaData result;
	result.totalNum = 0;
	result.rowNum = 0;
	result.colNum = 0;

	sf::Image image = texture.copyToImage();
	sf::Vector2u size = texture.getSize();
	sf::IntRect rectSourceSprite(0, 0, width, height);
	for (size_t i = 0; i < size.y; i += height, result.rowNum++)
	{
		rectSourceSprite.top = i;
		result.colNum = 0;
		for (size_t j = 0; j < size.x; j += width, result.colNum++)
		{
			rectSourceSprite.left = j;
			if (getSumPixels(image, rectSourceSprite) != 0)
			{
				result.totalNum++;
			}
			else
			{
				result.last = sf::Vector2u(result.rowNum, result.colNum - 1);
				break;
			}
		}
	}
	result.rowNum = size.y / height;
	result.colNum = size.x / width;
	return result;
}

void AnimatedSprite::setScale(sf::Vector2f scale)
{
	_sprite.setScale(scale);
}

sf::Vector2f AnimatedSprite::getLocalPosition()
{
	sf::FloatRect rect = _sprite.getGlobalBounds();
	return sf::Vector2f(rect.left + rect.width / 2, rect.top + rect.height / 2);
}

sf::Vector2f AnimatedSprite::getGlobalPosition()
{
	return _sprite.getPosition();
}

float AnimatedSprite::getRotation()
{
	return _sprite.getRotation() + _baseRotation;
}

void AnimatedSprite::setRotation(float angle)
{
	return _sprite.setRotation(angle + _baseRotation);
}

void AnimatedSprite::moveTo(sf::Vector2f target)
{
	sf::Vector2f pos = getLocalPosition();
	float distance = sqrt(pow(target.x - pos.x, 2) + pow(target.y - pos.y, 2));
	float rot = getRotation();
	float angle = DegToRad(rot - 180);
	if (distance < _speed)
	{
		return;
	}
	if (std::abs(angle) < M_PI / 2)
	{
		pos.x -= fabs(_speed * std::cos(angle));
	}
	else
	{
		if (abs(angle) > M_PI / 2)
		{
			pos.x += std::fabs(_speed * std::cos(angle));
		}
	}
	if (angle > 0 && angle < M_PI)
	{
		pos.y -= std::fabs(_speed * std::sin(angle));
	}
	else
	{
		if (angle < 0 || angle > M_PI)
		{
			pos.y += std::fabs(_speed * std::sin(angle));
		}
	}
	_sprite.setPosition(pos);
}

sf::FloatRect AnimatedSprite::GetGlobalBounds() const {
	return getTransform().transformRect(_sprite.getGlobalBounds());
}

void AnimatedSprite::MoveBySpeed()
{
	nextSprite();
	move(GetSpeed());
}

void AnimatedSprite::OnCollision(bool* destroy) {
	auto scene = m_scene.lock();

	if (!scene) {
		return;
	}

	*destroy = true;

	AnimatedSpriteSplash::Ptr splash = AnimatedSpriteSplash::Ptr(new AnimatedSpriteSplash(getPosition(), _splashFile));
	splash->setScale(_scaleSplash);
	scene->AddSplashObject(splash);
}

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(_sprite, states);
}

AnimatedSpriteSplash::AnimatedSpriteSplash(const sf::Vector2f& pos, std::string fileName)
{
	m_texture.loadFromFile(fileName);
	m_shape.setTexture(m_texture);

	setPosition(pos);

	auto bounds = m_shape.getGlobalBounds();
	m_shape.setOrigin(bounds.width * m_shape.getScale().x / 2, bounds.height * m_shape.getScale().y / 2);
}

void AnimatedSpriteSplash::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(m_shape, states);
}