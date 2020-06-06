#pragma once
#include "AlbumWindow.h"
#include <SFML/Graphics.hpp>
#include "SceneElements.h"

#define MAX_USERS 12

class DrawerDemo : public IDrawingThing
{
public:
	virtual ~DrawerDemo();

	// this function is called once when window is opened
	void Init(sf::RenderWindow* window) override;

	// this function is called periodically
	void Draw(sf::RenderWindow* window) override;

	// this function puts last screenshot in thread-safe way
	void PutFrame(const ImgConstPtr& img, uint32_t ts_ms) override;

	// this function puts layout update in thread-safe way
	void PutLayout(const LayoutInfo& l) override;

private:
	Scene::Ptr m_scene;

	ImgConstPtr m_img;
	sf::Texture m_txt[MAX_USERS];
	sf::Sprite m_spr[MAX_USERS];
	LayoutInfo m_layout;
};

