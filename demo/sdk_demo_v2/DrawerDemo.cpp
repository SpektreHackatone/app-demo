#include "DrawerDemo.h"

DrawerDemo::~DrawerDemo()
{
	OutputDebugString(L"WTTF!!!\n");
}

void DrawerDemo::Init()
{
}

void DrawerDemo::Draw(sf::RenderWindow* window)
{
	//OutputDebugString(L"draw started\n");

	m_spr.setTexture(m_txt, true);

	window->draw(m_spr);
}

void DrawerDemo::PutFrame(const ImgConstPtr& frame, uint32_t ts_ms)
{
	m_img = frame;
	if (!m_txt.loadFromImage(*m_img, sf::IntRect(0, 0, 500, 500))) {
		OutputDebugString(L"failed to load texture\n");
	}
}

void DrawerDemo::PutLayout(const Layout& layout)
{

}