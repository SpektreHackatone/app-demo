#pragma once
#include "AlbumWindow.h"
#include <SFML/Graphics.hpp>

class DrawerDemo : public IDrawingThing
{
public:
	virtual ~DrawerDemo();

	// this function is called once when window is opened
	void Init() override;

	// this function is called periodically
	void Draw(sf::RenderWindow* window) override;

	// this function puts last screenshot in thread-safe way
	void PutFrame(const ImgConstPtr& img, uint32_t ts_ms) override;

	// this function puts layout update in thread-safe way
	void PutLayout(const Layout& l) override;

private:
	ImgConstPtr m_img;
	sf::Texture m_txt;
	sf::Sprite m_spr;

	sf::CircleShape m_shape;
};

