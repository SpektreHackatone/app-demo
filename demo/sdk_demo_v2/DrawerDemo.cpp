#include "DrawerDemo.h"
#include "SceneElements.h"
#include "SceneLayoutImpls.h"

DrawerDemo::~DrawerDemo()
{
	OutputDebugString(L"WTTF!!!\n");
}

void DrawerDemo::Init(sf::RenderWindow* window)
{
	m_scene = Scene::Ptr(new Scene());

	auto size = window->getSize();
	BasicGridLayout::Ptr layout = BasicGridLayout::Ptr(new BasicGridLayout(size.x, size.y, 4, 2));
	layout->SetFillOrder(std::vector<int>{0, 7, 2, 5, 1, 6, 4, 3});

	Background::Ptr bg = Background::Ptr(new Background(sf::Vector2f(size.x, size.y)));
	bg->LoadFromFile("backgrounds/background1.jpg");

	m_scene->SetLayout(layout);
	m_scene->SetBackground(bg);

	LayoutInfo l(8);
	for (unsigned i = 0; i < 8; i++) {
		UserDrawable::Ptr user = layout->UserAt(i);
		l.push_back(UserInLayout{ i, L"Hello", RECT{0} });
	}
	m_layout = l;
}

void DrawerDemo::Draw(sf::RenderWindow* window)
{
	window->draw(*m_scene);
	//OutputDebugString(L"draw started\n");

	/*const int width = 400;
	const int height = 300;

	for (unsigned i = 0; i < MAX_USERS; i++) {
		if (m_txt->getSize().x == 0) {
			break;
		}

		float x = i * width;

		m_spr[i].setTexture(m_txt[i], true);
		m_spr[i].setPosition(x, 0.0);
		window->draw(m_spr[i]);
	}*/
}

void DrawerDemo::PutFrame(const ImgConstPtr& frame, uint32_t ts_ms)
{
	m_img = frame;
	auto layout = m_scene->GetLayout();

	for (unsigned i = 0; i < m_layout.size(); i++) {
		UserDrawable::Ptr user = layout->UserAt(i);

		sf::IntRect rect;
		const auto& l = m_layout[i];

		rect.left = l.rect.left;
		rect.top = l.rect.top;
		rect.width = l.rect.right - l.rect.left;
		rect.height = l.rect.bottom - l.rect.top;

		user->GetVideo()->SetImage(frame, rect);
	}
}

void DrawerDemo::PutLayout(const LayoutInfo& layout)
{
	m_layout = layout;
	m_scene->GetLayout()->Clear();
}