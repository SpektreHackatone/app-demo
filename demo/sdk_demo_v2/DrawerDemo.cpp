#include "DrawerDemo.h"
#include "SceneElements.h"
#include "SceneLayoutImpls.h"
#include "AnimatedSprite.h"

DrawerDemo::~DrawerDemo()
{
	OutputDebugString(L"WTTF!!!\n");
}

void DrawerDemo::Init(sf::RenderWindow* window)
{
	m_scene = Scene::Ptr(new Scene());

	auto size = window->getSize();
	m_windowSize = size;
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
		m_scene->AddCollidable(user);
	}
	m_layout = l;

	AnimatedSprite::Ptr tomato = AnimatedSprite::Ptr(new AnimatedSprite(m_scene,
		"C:\\Users\\winda\\Pictures\\tomato_128_128.png",
		sf::IntRect(0, 0, 128, 128),
		"C:\\Users\\winda\\Documents\\Adobe\\After Effects 2020\\Resources\\klaksa2.png",
		sf::Vector2f(0.1, 0.1)));
	tomato->setScale(0.2, 0.2);
	tomato->setPosition(m_windowSize.x / 2, m_windowSize.y / 2);
	tomato->SetSpeed(sf::Vector2f(-10, -7.5));
	/*
	SimpleTomato::Ptr tomato = SimpleTomato::Ptr(new SimpleTomato(m_scene));
	tomato->SetSpeed(sf::Vector2f(-2.0, -1.0));
	tomato->setPosition(m_windowSize.x / 2, m_windowSize.y / 2);
	*/
	m_scene->AddFlyingObject(tomato);
}

void DrawerDemo::Draw(sf::RenderWindow* window)
{
	window->draw(*m_scene);
	m_scene->UpdateCollisions();
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
	m_scene->ClearCollidable();

	for (unsigned i = 0; i < m_layout.size(); i++) {
		UserDrawable::Ptr user = m_scene->GetLayout()->UserAt(i);
		m_scene->AddCollidable(user);
	}
	AnimatedSprite::Ptr tomato = AnimatedSprite::Ptr(new AnimatedSprite(m_scene,
		"C:\\Users\\winda\\Pictures\\tomato_128_128.png",
		sf::IntRect(0, 0, 128, 128),
		"C:\\Users\\winda\\Documents\\Adobe\\After Effects 2020\\Resources\\klaksa2.png",
		sf::Vector2f(0.1, 0.1)));
	tomato->setScale(0.2, 0.2);
	tomato->setPosition(m_windowSize.x / 2, m_windowSize.y / 2);
	tomato->SetSpeed(sf::Vector2f(-10, -7.5));
	/*
	SimpleTomato::Ptr tomato = SimpleTomato::Ptr(new SimpleTomato(m_scene));
	tomato->SetSpeed(sf::Vector2f(-5, -2.5));
	tomato->setPosition(m_windowSize.x / 2, m_windowSize.y / 2);
	*/
	m_scene->AddFlyingObject(tomato);
}