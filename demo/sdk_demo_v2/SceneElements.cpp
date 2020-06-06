#include "SceneElements.h"

bool Collidable::Collides(const Collidable& other) const {
	return GetGlobalBounds().intersects(other.GetGlobalBounds());
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
	m_sprite.setTexture(m_txt, false);

	// scale vertically to fit in center
	float scale = m_size.y / rect.height;
	setScale(scale, scale);

	// move to fit in center
	setOrigin(rect.width / 2 - m_size.x / 2, 0);
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
			sp->OnCollision();
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