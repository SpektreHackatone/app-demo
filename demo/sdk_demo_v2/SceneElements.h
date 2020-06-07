#pragma once

#include <SFML/Graphics.hpp>
#include "DrawingCommon.h"
#include "LayoutGrabber.h"
#include <list>
#include "Menu.h"

class Collidable {
public:
	using Ptr = std::shared_ptr<Collidable>;
	using WeakPtr = std::weak_ptr<Collidable>;

	// bounding box in global coordinates
	virtual sf::FloatRect GetGlobalBounds() const {
		return sf::FloatRect();
	}

	// or you can implement your own Collides
	virtual bool Collides(const Collidable& other) const;
};

class Background : public sf::Drawable {
public:
	using Ptr = std::shared_ptr<Background>;

	Background(const sf::Vector2f& size) : m_size(size) {}

	void LoadFromFile(const std::string& path);
	void SetImage(const ImgConstPtr& image);

	void SetSize(const sf::Vector2f& size) {
		m_size = size;
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_size;

	ImgConstPtr m_image;

	static const size_t kNumRows = 2;
	static const size_t kNumColumns = 2;
	static const size_t kNumElems = kNumRows * kNumColumns;

	sf::Texture m_txt[kNumElems];
	sf::RenderTexture m_rtxt[kNumElems];
	sf::Sprite m_sprite[kNumElems];
	sf::Sprite m_rsprite[kNumElems];
};

class IFlyingObject : public sf::Drawable, public sf::Transformable, public Collidable {
public:
	using Ptr = std::shared_ptr<IFlyingObject>;

	void SetSpeed(const sf::Vector2f& speed) {
		m_speed = speed;
	}

	sf::Vector2f GetSpeed() const {
		return m_speed;
	}

	virtual void OnCollision(bool* destroy) = 0;

	virtual void MoveBySpeed() {
		m_prevPos = getPosition();
		move(GetSpeed());
	}

private:
	sf::Vector2f m_speed;
	sf::Vector2f m_prevPos;
};

class ISplashObject : public sf::Drawable, public sf::Transformable {
public:
	using Ptr = std::shared_ptr<ISplashObject>;
};

class IAnimatedObject : public sf::Drawable, public sf::Transformable
{
public:
	using Ptr = std::shared_ptr<IAnimatedObject>;

	virtual void UpdateObject() = 0;
	void NextStep() {
		if (clock.getElapsedTime().asMilliseconds() > m_rate)
		{
			m_cur_steps++;
			UpdateObject();
			clock.restart();
		}
	}
	void SetCurSteps(int cur_steps) {
		m_cur_steps = cur_steps;
	}
	int GetCurSteps() {
		return m_cur_steps;
	}
	void SetSteps(int steps) {
		m_steps = steps;
	}
	int GetSteps() {
		return m_steps;
	}
	void SetRate(int rate) {
		m_rate = rate;
	}
	int GetRate() {
		return m_rate;
	}

private:
	int m_steps;
	int m_cur_steps = 0;
	int m_rate = 0;
	sf::Clock clock;
};

// drawing of user elements

class IPhotoFrame : public sf::Drawable, public sf::Transformable {
public:
	using Ptr = std::shared_ptr<IPhotoFrame>;

	virtual sf::Vector2f GetVisibleSize() const = 0;
};

class UserVideo : public sf::Drawable, public sf::Transformable {
public:
	using Ptr = std::shared_ptr<UserVideo>;
	UserVideo(const sf::Vector2f& size) : m_visibleSize(size) {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void SetImage(const ImgConstPtr& image, const sf::IntRect& rect);
	void SetSize(const sf::Vector2f& size) {
		m_visibleSize = size;
	}

	sf::Vector2f GetRelativeCoords(const sf::Vector2f& v) const;

	sf::Vector2f GetOriginalOrigin() const {
		return m_origin;
	}

	void SetMirrored(bool mirror) {
		m_mirror = mirror;
	}

private:
	sf::Vector2f m_visibleSize;
	sf::Vector2f m_origin;

	ImgConstPtr m_image;
	sf::Texture m_txt;
	sf::Sprite m_sprite;

	bool m_mirror;
	float m_scale;
	sf::Vector2f m_rectSize;
};

class UserDrawable : public sf::Drawable, public sf::Transformable, public Collidable {
public:
	using Ptr = std::shared_ptr<UserDrawable>;

	UserDrawable(const sf::Vector2f& size, IPhotoFrame::Ptr frame = nullptr);

	// for Drawable
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// for Collidable
	sf::FloatRect GetGlobalBounds() const override;

	// origin of original video
	sf::Vector2f GetOriginalOrigin() const;

	void SetFrame(const IPhotoFrame::Ptr& frame) {
		m_frame = frame;
		SetVisibleSize(frame->GetVisibleSize());
	}

	IPhotoFrame::Ptr GetFrame() const {
		return m_frame;
	}

	UserVideo::Ptr GetVideo() {
		return m_video;
	}

	void SetVisibleSize(const sf::Vector2f& size) {
		m_visibleSize = size;
	}

	const UserInLayout& GetUserInLayoutInfo() const {
		return m_layoutInfo;
	}

	void SetUserInLayoutInfo(const UserInLayout& info) {
		m_layoutInfo = info;
		m_video->SetMirrored(!info.is_me);
	}

private:
	IPhotoFrame::Ptr m_frame;
	UserVideo::Ptr m_video;

	sf::Vector2f m_visibleSize;
	UserInLayout m_layoutInfo;
};

class ILayout : public sf::Drawable {
public:
	using Ptr = std::shared_ptr<ILayout>;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	UserDrawable::Ptr UserAt(int pos);

	size_t CountUsers() const {
		return m_users.size();
	}

	void Clear() {
		m_users.clear();
	}

protected:
	virtual sf::Vector2f GetPositionFor(int id) const = 0;
	virtual sf::Vector2f GetFrameSize() const = 0;
	virtual IPhotoFrame::Ptr MakeDefaultFrame() const {
		return nullptr;
	}

	std::vector<UserDrawable::Ptr> m_users;
};

class Scene : public sf::Drawable {
public:
	using Ptr = std::shared_ptr<Scene>;
	using WeakPtr = std::weak_ptr<Scene>;

	void SetMenu(const Menu::Ptr& menu) {
		m_menu = menu;
	}

	Menu::Ptr GetMenu() {
		return m_menu;
	}

	void SetLayout(const ILayout::Ptr& layout) {
		m_layout = layout;
	}

	ILayout::Ptr GetLayout() {
		return m_layout;
	}

	void SetBackground(const Background::Ptr& bg) {
		m_background = bg;
	}

	Background::Ptr GetBackground() {
		return m_background;
	}

	void AddSplashObject(const ISplashObject::Ptr& obj) {
		m_splashObjects.push_back(obj);
	}

	void ClearSplashes() {
		m_splashObjects.clear();
	}

	void AddAnimatedObject(const IAnimatedObject::Ptr& obj) {
		m_animatedObjects.push_back(obj);
	}
	void RemoveAnimatedObject(const IAnimatedObject::Ptr& obj) {
		auto& it = std::find(m_animatedObjects.cbegin(), m_animatedObjects.cend(), obj);

		if (it != m_animatedObjects.cend()) {
			m_animatedObjects.erase(it);
		}
	}

	void AddFlyingObject(const IFlyingObject::Ptr& obj);
	void RemoveFlyingObject(const IFlyingObject::Ptr& obj);

	void AddCollidable(const Collidable::WeakPtr& c) {
		m_collidableObjects.push_back(c);
	}
	void ClearCollidable() {
		m_collidableObjects.clear();
	}

	void UpdateCollisions();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
	bool CheckCollision(const Collidable::Ptr& obj);
	bool CheckAnimation(const IAnimatedObject::Ptr& obj);

	ILayout::Ptr m_layout;
	Background::Ptr m_background;
	Menu::Ptr m_menu;

	std::list<IFlyingObject::Ptr> m_flyingObjects;
	std::map<IFlyingObject::Ptr, bool> m_collisionFrees;
	std::list<Collidable::WeakPtr> m_collidableObjects;
	std::list<ISplashObject::Ptr> m_splashObjects;
	std::list<IAnimatedObject::Ptr> m_animatedObjects;
};