#pragma once

#include <SFML/Graphics.hpp>
#include "DrawingCommon.h"
#include <list>

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

class Background : public sf::Drawable, public sf::Transformable {
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
	void ScaleAndFit();

	sf::Vector2f m_size;

	ImgConstPtr m_image;

	static const size_t kNumRows = 4;
	static const size_t kNumColumns = 4;
	static const size_t kNumElems = kNumRows * kNumColumns;

	sf::Texture m_txt[kNumElems];
	sf::Sprite m_sprite[kNumElems];
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

	virtual void OnCollision() = 0;

	virtual void MoveBySpeed() {
		move(GetSpeed());
	}

private:
	sf::Vector2f m_speed;
};

class ISplashObject : public sf::Drawable, public sf::Transformable {
public:
	using Ptr = std::shared_ptr<ISplashObject>;
};

// drawing of user elements

class IPhotoFrame : public sf::Drawable, public sf::Transformable {
public:
	using Ptr = std::shared_ptr<IPhotoFrame>;
};

class UserVideo : public sf::Drawable, public sf::Transformable {
public:
	using Ptr = std::shared_ptr<UserVideo>;
	UserVideo(const sf::Vector2f& size) : m_size(size) {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void SetImage(const ImgConstPtr& image, const sf::IntRect& rect);
	void SetSize(const sf::Vector2f& size) {
		m_size = size;
	}

private:
	sf::Vector2f m_size;

	ImgConstPtr m_image;
	sf::Texture m_txt;
	sf::Sprite m_sprite;
};

class UserDrawable : public sf::Drawable, public sf::Transformable, public Collidable {
public:
	using Ptr = std::shared_ptr<UserDrawable>;

	UserDrawable(const sf::Vector2f& size, IPhotoFrame::Ptr frame = nullptr);

	// for Drawable
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// for Collidable
	sf::FloatRect GetGlobalBounds() const override;

	void SetFrame(const IPhotoFrame::Ptr& frame) {
		m_frame = frame;
	}

	IPhotoFrame::Ptr GetFrame() const {
		return m_frame;
	}

	UserVideo::Ptr GetVideo() {
		return m_video;
	}

	void SetSize(const sf::Vector2f& size) {
		m_size = size;
	}

private:
	IPhotoFrame::Ptr m_frame;
	UserVideo::Ptr m_video;

	sf::Vector2f m_size;
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

	void AddFlyingObject(const IFlyingObject::Ptr& obj);
	void RemoveFlyingObject(const IFlyingObject::Ptr& obj);

	void AddCollidable(const Collidable::WeakPtr& c) {
		m_collidableObjects.push_back(c);
	}

	void UpdateCollisions();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
	bool CheckCollision(const Collidable::Ptr& obj);

	ILayout::Ptr m_layout;
	Background::Ptr m_background;

	std::list<IFlyingObject::Ptr> m_flyingObjects;
	std::list<Collidable::WeakPtr> m_collidableObjects;
	std::list<ISplashObject::Ptr> m_splashObjects;
};