#include "AnimatedSprite.h"
#include "VectorHelper.h"
#include <math.h>

AnimatedSprite::AnimatedSprite(
	const Scene::WeakPtr& scene,
	std::string spriteFile,
	sf::IntRect spriteRect,
	const ISplashObject::Ptr& splash,
	const IAnimatedObject::Ptr& effect,
	float baseRotation,
	float speed)
	:_baseRotation(baseRotation),
	_current(0),
	_speed(speed),
	m_scene(scene),
	_splash(splash),
	_effect(effect),
	_rect(spriteRect),
	m_filename(spriteFile)
{
	if (!_texture.loadFromFile(spriteFile)) {
		OutputDebugString(L"failed to load texture\n");
	}
	_sprite.setTexture(_texture);
	_sprite.setTextureRect(_rect);

	_meta = getSpriteMetaData(*_sprite.getTexture(), _rect.width, _rect.height);

	auto b = _sprite.getGlobalBounds();

	_sprite.setOrigin(sf::Vector2f(b.width / 2, b.height / 2));
}

void AnimatedSprite::nextSprite()
{
	if (_meta.totalNum != 0) {
		_sprite.setTextureRect(_rect);
		_current = (_current + 1) % _meta.totalNum;
		_rect.left = (_current % _meta.colNum) * _rect.width;
		_rect.top = (_current / _meta.colNum) * _rect.height;
	}
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


sf::FloatRect AnimatedSprite::GetGlobalBounds() const {
	return getTransform().transformRect(_sprite.getGlobalBounds());
}

void AnimatedSprite::MoveBySpeed()
{
	nextSprite();
	auto speedN = Normalize(GetSpeed());
	setRotation(RadToDeg(std::atan2(speedN.y, speedN.x)) - _baseRotation);
	move(GetSpeed());
}

void AnimatedSprite::OnCollision(bool* destroy) {
	auto scene = m_scene.lock();

	if (!scene) {
		return;
	}

	*destroy = true;
	if (_splash != nullptr)
	{
		_splash->setPosition(getPosition());
		scene->AddSplashObject(_splash);
		_splash.reset();
	}
	if (_effect != nullptr)
	{
		_effect->setPosition(getPosition());
		scene->AddAnimatedObject(_effect);
		_effect.reset();
	}
}

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(_sprite, states);
}

AnimatedSpriteSplash::AnimatedSpriteSplash(std::string fileName)
{
	m_texture.loadFromFile(fileName);
	m_shape.setTexture(m_texture);

	auto bounds = m_shape.getGlobalBounds();
	m_shape.setOrigin(bounds.width * m_shape.getScale().x / 2, bounds.height * m_shape.getScale().y / 2);
}

void AnimatedSpriteSplash::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(m_shape, states);
}

AnimatedEffect::AnimatedEffect(std::string fileName, sf::IntRect rect, int from, int to)
	: m_rect(rect)
{
	SetCurSteps(from);
	SetSteps(to);
	m_texture.loadFromFile(fileName);
	m_shape.setTexture(m_texture);
	m_shape.setTextureRect(m_rect);
	m_meta = getSpriteMetaData();

	auto bounds = m_shape.getGlobalBounds();
	m_shape.setOrigin(bounds.width * m_shape.getScale().x / 2, bounds.height * m_shape.getScale().y / 2);
}

void AnimatedEffect::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_shape, states);
}

void AnimatedEffect::UpdateObject()
{
	m_rect.left = ((GetCurSteps() % m_meta.totalNum) % m_meta.colNum) * m_rect.width;
	m_rect.top = ((GetCurSteps() % m_meta.totalNum) / m_meta.colNum) * m_rect.height;
	m_shape.setTextureRect(m_rect);
}

int AnimatedEffect::getSumPixels(sf::Image img, sf::IntRect rect)
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

SpriteMetaData AnimatedEffect::getSpriteMetaData()
{
	SpriteMetaData result;
	result.totalNum = 0;
	result.rowNum = 0;
	result.colNum = 0;

	sf::Image image = m_texture.copyToImage();
	sf::Vector2u size = m_texture.getSize();
	sf::IntRect rectSourceSprite(0, 0, m_rect.width, m_rect.height);
	for (size_t i = 0; i < size.y; i += m_rect.height, result.rowNum++)
	{
		rectSourceSprite.top = i;
		result.colNum = 0;
		for (size_t j = 0; j < size.x; j += m_rect.width, result.colNum++)
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
	result.rowNum = size.y / m_rect.height;
	result.colNum = size.x / m_rect.width;
	return result;
}