#pragma once
#include "SceneElements.h"
#include "AnimatedSprite.h"

class MenuBlock : public sf::Drawable
{
public:
	MenuBlock(const Scene::WeakPtr& scene);
	void update(sf::RenderTarget&);
	void setPosition(sf::Vector2f, float, sf::FloatRect);
	void load(std::string);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	int selectedIndex;
	std::shared_ptr<sf::Text> txt;
	std::shared_ptr<sf::Font> font;
	std::shared_ptr<AnimatedSprite> selectedSprite;
	std::shared_ptr<sf::Sprite> left;
	std::shared_ptr<sf::Texture> leftTexture;
	std::shared_ptr<sf::Sprite> right;
	std::shared_ptr<sf::Texture> rightTexture;
	std::vector<std::shared_ptr<AnimatedSprite>> sprites;
private:
	Scene::WeakPtr m_scene;
};

class Menu : public sf::Drawable
{
public:
	using Ptr = std::shared_ptr<Menu>;

	Menu(const Scene::WeakPtr& scene);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update(sf::RenderTarget& target);

private:
	Scene::WeakPtr m_scene;
	sf::RectangleShape _workspace;
	std::vector<MenuBlock> _blocks;
	sf::Texture _textureBtn;
	sf::Sprite _btn;
	bool _isActive;
	sf::Clock clock;
};