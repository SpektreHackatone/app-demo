#include "Menu.h"
#include <Windows.h>

MenuBlock::MenuBlock(const Scene::WeakPtr& scene)
	: txt(std::make_shared<sf::Text>()),
	font(std::make_shared<sf::Font>()),
	left(std::make_shared<sf::Sprite>()),
	right(std::make_shared<sf::Sprite>()),
	leftTexture(std::make_shared<sf::Texture>()),
	rightTexture(std::make_shared<sf::Texture>()),
	m_scene(scene)
{
	font->loadFromFile("Caveat-Regular.ttf");
	txt->setFont(*font);
	txt->setCharacterSize(24);
	txt->setFillColor(sf::Color::Black);
	leftTexture->loadFromFile("images\\left.png");
	left->setTexture(*leftTexture);
	left->setScale(sf::Vector2f(0.1, 0.1));
	rightTexture->loadFromFile("images\\right.png");
	right->setTexture(*rightTexture);
	right->setScale(sf::Vector2f(0.1, 0.1));
	selectedIndex = 0;
}

void MenuBlock::update(sf::RenderTarget& window)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// transform the mouse position from window coordinates to world coordinates
		sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition());

		sf::FloatRect lgb = left->getGlobalBounds();
		if (lgb.contains(mouse))
		{
			if (selectedIndex > 0)
			{
				selectedIndex--;
				selectedSprite = std::shared_ptr<AnimatedSprite>(sprites[selectedIndex]);
			}
		}
		else
		{
			sf::FloatRect rbg = right->getGlobalBounds();
			if (rbg.contains(mouse))
			{
				//clicking in frame right
				if (selectedIndex < sprites.size() - 1)
				{
					selectedIndex++;
					selectedSprite = std::shared_ptr<AnimatedSprite>(sprites[selectedIndex]);
				}
			}
		}
	}
}

void MenuBlock::setPosition(sf::Vector2f basePosition, float padding, sf::FloatRect workspaceSize)
{
	sf::FloatRect flPos = left->getLocalBounds();
	left->setOrigin(
		flPos.left + flPos.width / 2.0f,
		flPos.top + flPos.height / 2.0f);
	left->setPosition(
		workspaceSize.left + flPos.width * left->getScale().x,
		basePosition.y - flPos.height * left->getScale().y - padding);

	sf::FloatRect frPos = right->getLocalBounds();
	right->setOrigin(
		frPos.left + frPos.width / 2.0f,
		frPos.top + frPos.height / 2.0f);
	right->setPosition(
		workspaceSize.left + workspaceSize.width - frPos.width * right->getScale().x,
		basePosition.y - frPos.height * right->getScale().y - padding);

	sf::FloatRect ftPos = txt->getLocalBounds();
	txt->setOrigin(
		ftPos.left + ftPos.width / 2.0f,
		ftPos.top + ftPos.height / 2.0f);
	txt->setPosition(
		basePosition.x,
		basePosition.y - frPos.height * right->getScale().y - padding - ftPos.height - padding);

	/*for (auto& sprite : sprites)
	{
		auto sPos = sprite->getLocalBounds();
		sprite->setOrigin(
			sPos.left + sPos.width / 2.0f,
			sPos.top + sPos.height / 2.0f);
		sprite->setScale(getScaling(sprite->getScale(), sprite->getSize(), sf::Vector2f(flPos.left + flPos.width * left->getScale().x, frPos.top + frPos.height * right->getScale().y)));
		sprite->setPosition(
			basePosition.x,
			basePosition.y - sprite->getLocalBounds().height * sprite->getScale().y - padding);
	}*/
}

void MenuBlock::load(std::string path)
{
	std::wstring pattern(path.begin(), path.end());
	pattern.append(L"\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	do
	{
		hFind = FindFirstFile(pattern.c_str(), &data);
		sf::IntRect rect;
		rect.left = 0;
		rect.top = 0;
		std::wstring w_file(data.cFileName);
		std::string filename(w_file.begin(), w_file.end());

		//tomato_128_128_5_6_0.png
		//background1_1920_1440_1_1_0.jpg
		//frame1_514_663_1_1_0.png

		std::string delimiter = "_";

		size_t pos = filename.find(delimiter);
		std::string name = filename.substr(0, pos);
		filename.erase(0, pos + delimiter.length());

		pos = filename.find(delimiter);
		std::string width = filename.substr(0, pos);
		filename.erase(0, pos + delimiter.length());

		pos = filename.find(delimiter);
		std::string height = filename.substr(0, pos);
		filename.erase(0, pos + delimiter.length());

		pos = filename.find(delimiter);
		std::string col = filename.substr(0, pos);
		filename.erase(0, pos + delimiter.length());

		pos = filename.find(delimiter);
		std::string row = filename.substr(0, pos);
		filename.erase(0, pos + delimiter.length());

		pos = filename.find(delimiter);
		std::string rotate = filename.substr(0, pos);
		filename.erase(0, pos + delimiter.length());

		rect.width = std::stoi(width);
		rect.height = std::stoi(height);

		sprites.push_back(std::make_shared<AnimatedSprite>(
			m_scene,
			std::string(w_file.begin(), w_file.end()),
			rect,
			"",
			sf::Vector2f(0, 0),
			std::stoi(rotate),
			5));

	} while (FindNextFile(hFind, &data) != 0);
}

void MenuBlock::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(*txt);
	target.draw(*left);
	target.draw(*right);
	if (selectedSprite->isInit())
	{
		selectedSprite->draw(target, states);
	}
}

Menu::Menu(const Scene::WeakPtr& scene)
	: _isActive(false),
	m_scene(scene)
{
	_textureBtn.loadFromFile("images/menu_btn.png");
	_btn.setTexture(_textureBtn);
	_btn.setPosition(10, 10);
	_btn.setScale(0.2, 0.2);
	_btn.setColor(sf::Color(75, 170, 211, 128));

	_workspace.setFillColor(sf::Color(75, 170, 211, 64));

	_blocks.push_back(MenuBlock(scene));
	_blocks[0].txt->setString(L"Рамка");
	_blocks[0].load("images/frames");
	_blocks[0].selectedSprite = std::shared_ptr<AnimatedSprite>(_blocks[0].sprites[0]);
	_blocks.push_back(MenuBlock(scene));
	_blocks[1].txt->setString(L"Эффект");
	_blocks[1].load("images/effects");
	_blocks[1].selectedSprite = std::shared_ptr<AnimatedSprite>(_blocks[1].sprites[0]);
	_blocks.push_back(MenuBlock(scene));
	_blocks[2].txt->setString(L"Фон");
	_blocks[2].load("images/backgrounds");
	_blocks[2].selectedSprite = std::shared_ptr<AnimatedSprite>(_blocks[2].sprites[0]);
}

void Menu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (_isActive)
	{
		target.draw(_workspace);
		for (auto& block : _blocks)
		{
			block.draw(target, states);
		}
	}
	else
	{
		target.draw(_btn);
	}
}

void Menu::update(sf::RenderTarget& target)
{
	if (clock.getElapsedTime().asMilliseconds() > 50.0f)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2f mouse = target.mapPixelToCoords(sf::Mouse::getPosition());
			if (_isActive)
			{
				// retrieve the bounding box of the sprite
				sf::FloatRect bounds = _workspace.getGlobalBounds();
				// hit test
				if (bounds.contains(mouse))
				{
					//clicking in menu field
					for (auto& block : _blocks)
					{
						block.update(target);
					}
				}
				else
					_isActive = false;
			}
			else
			{
				// retrieve the bounding box of the sprite
				sf::FloatRect bounds = _btn.getGlobalBounds();
				// hit test
				if (bounds.contains(mouse))
				{
					//show menu field
					sf::Vector2u wSize = target.getSize();
					_workspace.setSize(sf::Vector2f(200, wSize.y - 20));
					_workspace.setPosition(10, 10);
					sf::Vector2f wgPos = _workspace.getPosition();
					sf::FloatRect wlPos = _workspace.getLocalBounds();
					wlPos.left = wgPos.x;
					wlPos.top = wgPos.y;
					float padding = 20;
					for (int i = 0; i < _blocks.size(); i++)
					{
						_blocks[i].setPosition(sf::Vector2f(wgPos.x + wlPos.width / 2, wgPos.y + ((i + 1) * wlPos.height) / _blocks.size()), padding, wlPos);
					}
					_isActive = true;
				}
			}
		}
		clock.restart();
	}
}