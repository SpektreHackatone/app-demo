#include "DrawerDemo.h"
#include "SceneElements.h"
#include "SceneLayoutImpls.h"
#include "AnimatedSprite.h"
#include "TomatoLaunchApi.h"
#include "LayoutGrabber.h"

DrawerDemo::DrawerDemo()
	: m_detector()
	, m_detectorInitialized(false) {}

DrawerDemo::~DrawerDemo()
{
	OutputDebugString(L"WTTF!!!\n");
}

void DrawerDemo::Init(sf::RenderWindow* window)
{
	m_scene = Scene::Ptr(new Scene());

	auto size = window->getSize();
	m_windowSize = size;
	BasicGridLayout::Ptr layout = BasicGridLayout::Ptr(new BasicGridLayout(size.x, size.y, 3, 2));
	layout->SetFillOrder(std::vector<int>{0, 2, 4, 3, 1, 5});

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

	AnimatedEffect::Ptr explosion = AnimatedEffect::Ptr(new AnimatedEffect("images/explosion0_71_100_5_5.png", sf::IntRect(0, 0, 71, 100), 8, 22));

	AnimatedSpriteSplash::Ptr tomato_splash = AnimatedSpriteSplash::Ptr(new AnimatedSpriteSplash("images/grunge0.png"));
	tomato_splash->setScale(0.3, 0.3);
	AnimatedSprite::Ptr tomato = AnimatedSprite::Ptr(new AnimatedSprite(
		m_scene,
		"images/fireballsm_128_146_6_5.png",
		sf::IntRect(0, 0, 128, 146),
		tomato_splash,
		explosion,
		90));
	tomato->setScale(0.3, 0.3);

	tomato->setPosition(m_windowSize.x / 2, m_windowSize.y * 3 / 4);
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

		if (m_detectorInitialized && user->GetUserInLayoutInfo().is_me) {
			cv::Size size;
			size.width = frame->getSize().x;
			size.height = frame->getSize().y;
			cv::Mat cv_img(size, CV_8UC4, (void*)frame->getPixelsPtr());
			cv::Mat roi(cv_img(cv::Rect(rect.left, rect.top, rect.width, rect.height)));

			m_detector.Detect(roi, ts_ms,
				[this](MDEventType ev, cv::Point p1, cv::Point p2) { OnMotionDetected(ev, p1, p2); });
		}
	}
}

void DrawerDemo::PutLayout(const LayoutInfo& layout)
{
	m_layout = layout;
	m_scene->GetLayout()->Clear();
	m_scene->ClearCollidable();

	for (unsigned i = 0; i < m_layout.size(); i++) {
		UserDrawable::Ptr user = m_scene->GetLayout()->UserAt(i);
		user->SetUserInLayoutInfo(m_layout[i]);
		m_scene->AddCollidable(user);

		if (!m_detectorInitialized) {
			const auto& sz = m_layout[i].rect;
			m_detectorInitialized = true;
			m_detector.SetRects(getStandart2Rects(cv::Size(sz.right - sz.left, sz.bottom - sz.top)));
		}
	}

	AnimatedSpriteSplash::Ptr tomato_splash = AnimatedSpriteSplash::Ptr(new AnimatedSpriteSplash("images/klaksa2.png"));
	tomato_splash->setScale(0.1, 0.1);
	AnimatedSprite::Ptr tomato = AnimatedSprite::Ptr(new AnimatedSprite(m_scene,
		"images/tomato_128_128.png",
		sf::IntRect(0, 0, 128, 128),
		tomato_splash,
		AnimatedEffect::Ptr(nullptr)));
	tomato->setScale(0.2, 0.2);
	tomato->setPosition(m_windowSize.x / 2, m_windowSize.y / 2);
	tomato->SetSpeed(sf::Vector2f(-10, -7.5));
}

void DrawerDemo::PutChatMessage(const std::wstring& str)
{
	OutputDebugString(str.c_str());
	// tomato launcher!
	if (str[0] == L'$' && str[1] == 'L') {
		IFlyingObject::Ptr tomato = TomatoLaunchApi::GetTomatoFromMsg(m_scene, str);
		m_scene->AddFlyingObject(tomato);
	}
}

IFlyingObject::Ptr DrawerDemo::SpawnTomato(const cv::Point& p1) {
	UserDrawable::Ptr user;
	for (unsigned i = 0; i < m_scene->GetLayout()->CountUsers(); i++) {
		user = m_scene->GetLayout()->UserAt(i);
		if (user->GetUserInLayoutInfo().is_me) {
			break;
		}
	}

	if (!user->GetUserInLayoutInfo().is_me) {
		OutputDebugString(L"cant find myself to spawn tomato");
		return nullptr;
	}

	sf::Vector2f pos = user->GetVideo()->GetRelativeCoords(sf::Vector2f(p1.x, p1.y));

	float pos_x = user->getPosition().x + pos.x;
	float pos_y = user->getPosition().y + pos.y;

	// FIXME: use tomato factory instead
	AnimatedSpriteSplash::Ptr tomato_splash = AnimatedSpriteSplash::Ptr(new AnimatedSpriteSplash("images/klaksa2.png"));
	tomato_splash->setScale(0.3, 0.3);
	AnimatedSprite::Ptr tomato = AnimatedSprite::Ptr(new AnimatedSprite(m_scene,
		"images/tomato_128_128.png",
		sf::IntRect(0, 0, 128, 128),
		tomato_splash,
		AnimatedEffect::Ptr(nullptr)));
	tomato->setScale(0.3, 0.3);
	tomato->setPosition(pos_x, pos_y);
	tomato->SetSpeed(sf::Vector2f(0, 0));

	m_scene->AddFlyingObject(tomato);

	return tomato;
}

IFlyingObject::Ptr DrawerDemo::SpawnFireball(const cv::Point& p1) {
	UserDrawable::Ptr user;
	for (unsigned i = 0; i < m_scene->GetLayout()->CountUsers(); i++) {
		user = m_scene->GetLayout()->UserAt(i);
		if (user->GetUserInLayoutInfo().is_me) {
			break;
		}
	}

	if (!user->GetUserInLayoutInfo().is_me) {
		OutputDebugString(L"cant find myself to spawn tomato");
		return nullptr;
	}

	sf::Vector2f pos = user->GetVideo()->GetRelativeCoords(sf::Vector2f(p1.x, p1.y));

	float pos_x = user->getPosition().x + pos.x;
	float pos_y = user->getPosition().y + pos.y;

	AnimatedSpriteSplash::Ptr grunge = AnimatedSpriteSplash::Ptr(new AnimatedSpriteSplash("images/grunge0.png"));
	grunge->setScale(0.3, 0.3);

	AnimatedEffect::Ptr explosion = AnimatedEffect::Ptr(new AnimatedEffect("images/explosion0_71_100_5_5.png", sf::IntRect(0, 0, 71, 100), 8, 22));

	AnimatedSprite::Ptr fireball = AnimatedSprite::Ptr(new AnimatedSprite(
		m_scene,
		"images/fireballsm_128_146_6_5.png",
		sf::IntRect(0, 0, 128, 146),
		grunge,
		explosion,
		90));
	fireball->setScale(0.3, 0.3);
	fireball->setPosition(pos_x, pos_y);
	fireball->SetSpeed(sf::Vector2f(0, 0));

	m_scene->AddFlyingObject(fireball);

	return fireball;
}

void DrawerDemo::LaunchTomato(const IFlyingObject::Ptr& obj, const cv::Point& p1, const cv::Point& p2) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;

	const float scale = 0.5;

	// send message to chat instead of actual launching
	// so everyone can enjoy tomato
	m_scene->RemoveFlyingObject(obj);

	std::wstring msg = TomatoLaunchApi::LaunchTomatoMsg(obj, dx * scale, dy * scale);
	m_outputQueue.push_back(msg);
}

void DrawerDemo::OnMotionDetected(MDEventType ev, cv::Point p1, cv::Point p2)
{
	OutputDebugString(L"Motion detected\n");
	switch (ev) {
	case MDEventType::IN1:
		if (!m_leftLauncher) {
			m_leftLauncher = SpawnFireball(p1);
		}
		break;
	case MDEventType::IN2:
		if (!m_rightLauncher) {
			m_rightLauncher = SpawnTomato(p1);
		}
		break;
	case MDEventType::OUT1:
		if (m_leftLauncher) {
			LaunchTomato(m_leftLauncher, p1, p2);
			m_leftLauncher = nullptr;
		}
		break;
	case MDEventType::OUT2:
		if (m_rightLauncher) {
			LaunchTomato(m_rightLauncher, p1, p2);
			m_rightLauncher = nullptr;
		}
		break;
	case MDEventType::IN12:
	case MDEventType::OUT12:
		break;
	}
}

std::list<std::wstring>&& DrawerDemo::TakeOutputChatQueue() {
	return std::move(m_outputQueue);
}