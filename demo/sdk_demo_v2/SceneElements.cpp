#include "SceneElements.h"
#include <Windows.h>

bool Collidable::Collides(const Collidable& other) const {
	return GetGlobalBounds().intersects(other.GetGlobalBounds());
}

// #############################################################################
// Background

void Background::LoadFromFile(const std::string& path) {
	ImgPtr img = ImgPtr(new sf::Image());
	if (!img->loadFromFile(path)) {
		OutputDebugString(L"failed to load background\n");
		return;
	}

	SetImage(img);
}

void Background::SetImage(const ImgConstPtr& image) {
	m_image = image;

	size_t width = m_image->getSize().x;
	size_t height = m_image->getSize().y;

	size_t p_width = m_image->getSize().x / kNumColumns;
	size_t p_height = m_image->getSize().y / kNumRows;

	const float scale_x = m_size.x / float(width);
	const float scale_y = m_size.y / float(height);

	float fin_scale = std::max(scale_x, scale_y);

	// get offset
	sf::Vector2f offset;

	offset.x = -(width * fin_scale - m_size.x) / 2;
	offset.y = -(height * fin_scale - m_size.y) / 2;

	// fill in textures
	for (unsigned i = 0; i < kNumRows; i++) {
		for (unsigned j = 0; j < kNumColumns; j++) {
			const size_t idx = i * kNumColumns + j;
			sf::IntRect rect(j * p_width, i * p_height, p_width, p_height);

			if (!m_txt[idx].loadFromImage(*image, rect)) {
				OutputDebugString(L"failed to load background fragment");
			}

			m_txt[idx].setSmooth(true);

			m_sprite[idx].setTexture(m_txt[idx]);
			m_sprite[idx].setScale(fin_scale, fin_scale);

			m_rtxt[idx].create(p_width * fin_scale, p_height * fin_scale);
			m_rtxt[idx].draw(m_sprite[idx]);
			m_rtxt[idx].display();

			m_rsprite[idx].setTexture(m_rtxt[idx].getTexture(), true);
			m_rsprite[idx].setPosition(j * p_width * fin_scale + offset.x, i * p_height * fin_scale + offset.y);
		}
	}
}

void Background::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (size_t i = 0; i < kNumElems; i++) {
		target.draw(m_rsprite[i], states);
	}
}

// #############################################################################
// User drawable things

void UserVideo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(m_sprite, states);
}

void UserVideo::SetImage(const ImgConstPtr& image, const sf::IntRect& rect) {
	m_image = image;
	m_txt.loadFromImage(*image, rect);
	m_txt.setSmooth(true);
	m_sprite.setTexture(m_txt);

	// scale vertically to fit in center
	float scale = m_size.y / rect.height;
	setScale(scale, scale);

	sf::IntRect crop;
	crop.top = 0;
	crop.left = std::round(rect.width / 2 - m_size.x / scale / 2);
	crop.width = std::round(m_size.x / scale);
	crop.height = m_size.y;

	m_sprite.setTextureRect(crop);
}

// #############################################################################
// User drawable things

UserDrawable::UserDrawable(const sf::Vector2f& size, IPhotoFrame::Ptr frame)
	: m_size(size)
	, m_frame(frame) {
	m_video = UserVideo::Ptr(new UserVideo(size));
}

sf::FloatRect UserDrawable::GetGlobalBounds() const {
	// FIXME
	sf::FloatRect ret;

	const auto pos = getPosition();

	ret.left = pos.x;
	ret.top = pos.y;
	ret.width = m_size.x;
	ret.height = m_size.y;

	return ret;
}

void UserDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// How to draw a user
	if (!m_video) {
		return;
	}

	states.transform *= getTransform();

	// 1. Draw his pretty face
	target.draw(*m_video, states);

	// 2. Draw a frame
	if (m_frame) {
		target.draw(*m_frame, states);
	}
}

// #############################################################################
// Layout common things
void ILayout::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (const auto& user : m_users) {
		if (user) {
			target.draw(*user, states);
		}
	}
}

UserDrawable::Ptr ILayout::UserAt(int pos) {
	if (m_users.size() <= pos) {
		m_users.resize(pos + 1);
	}

	if (!m_users[pos]) {
		m_users[pos] = UserDrawable::Ptr(new UserDrawable(GetFrameSize(), MakeDefaultFrame()));
		m_users[pos]->setPosition(GetPositionFor(pos));
	}

	return m_users[pos];
}

// #############################################################################
// Scene drawing

void Scene::AddFlyingObject(const IFlyingObject::Ptr& obj) {
	m_flyingObjects.push_back(obj);
}

void Scene::RemoveFlyingObject(const IFlyingObject::Ptr& obj) {
	auto& it = std::find(m_flyingObjects.cbegin(), m_flyingObjects.cend(), obj);

	if (it != m_flyingObjects.cend()) {
		m_flyingObjects.erase(it);
	}
}

void Scene::UpdateCollisions() {
	for (auto& it = m_flyingObjects.begin(); it != m_flyingObjects.end(); it++) {
		const IFlyingObject::Ptr sp = *it;

		if (CheckCollision(sp)) {
			bool destroy = false;
			sp->OnCollision(&destroy);

			if (destroy) {
				const auto d_it = it;
				it++;
				m_flyingObjects.erase(d_it);
			}
		}
		else {
			sp->MoveBySpeed();
		}
	}
}

bool Scene::CheckCollision(const Collidable::Ptr& obj)
{
	for (auto& it = m_collidableObjects.cbegin(); it != m_collidableObjects.cend(); it++) {
		const Collidable::WeakPtr& wp = *it;

		if (auto sp = wp.lock()) {
			if (sp->Collides(*obj)) {
				return true;
			}
		}
		else {
			// remove expired collidable object out from here
			m_collidableObjects.erase(it);
		}
	}

	return false;
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// How to cook a good scene

	// 1. Draw background
	if (m_background) {
		target.draw(*m_background, states);
	}

	// 2. Draw layout (with user pictures)
	if (m_layout) {
		target.draw(*m_layout, states);
	}

	// 3. Draw splashes
	for (const auto& obj : m_splashObjects) {
		target.draw(*obj, states);
	}

	// 4. Draw flying objects
	for (const auto& obj : m_flyingObjects) {
		target.draw(*obj, states);
	}

	// TODO: apply effects maybe
}