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

	const int width = 400;
	const int height = 300;

	for (unsigned i = 0; i < MAX_USERS; i++) {
		if (m_txt->getSize().x == 0) {
			break;
		}

		float x = i * width;

		m_spr[i].setTexture(m_txt[i], true);
		m_spr[i].setPosition(x, 0.0);
		window->draw(m_spr[i]);
	}
}

void DrawerDemo::PutFrame(const ImgConstPtr& frame, uint32_t ts_ms)
{
	m_img = frame;

	for (unsigned i = 0; i < m_layout.size(); i++) {
		sf::IntRect rect;
		const auto& l = m_layout[i];

		rect.left = l.rect.left;
		rect.top = l.rect.top;
		rect.width = l.rect.right - l.rect.left;
		rect.height = l.rect.bottom - l.rect.top;

		if (!m_txt[i].loadFromImage(*m_img, rect)) {
			OutputDebugString(L"failed to load texture\n");
		}
	}
}

void DrawerDemo::PutLayout(const Layout& layout)
{
	m_layout = layout;
}