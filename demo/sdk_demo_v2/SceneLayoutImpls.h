#pragma once

#include "SceneElements.h"

class BasicPhotoFrame : public IPhotoFrame {
public:
	BasicPhotoFrame(const sf::Vector2f& size) {
		SetSize(size);
	}

	void SetSize(const sf::Vector2f& size);
	sf::Vector2f GetVisibleSize() const override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::RectangleShape m_shape;
};

class TexturePhotoFrame : public IPhotoFrame {
public:
	TexturePhotoFrame(const std::string& filename,
					  const sf::Vector2f& frameVisibleSize,
					  const sf::Vector2f& frameScale);

	sf::Vector2f GetVisibleSize() const override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Texture m_txt;
	sf::Sprite m_sprite;

	sf::Vector2f m_visibleSize;
};

// ##########################################################################################################
// layouts

class BasicGridLayout : public ILayout {
public:
	using Ptr = std::shared_ptr<BasicGridLayout>;

	BasicGridLayout(int width, int height, int num_cols, int num_rows, float x_scale = 0.7, float y_scale = 0.75);

	void SetFillOrder(const std::vector<int>& order) {
		m_fillOrder = order;
	}

protected:
	sf::Vector2f GetPositionFor(int i) const override;
	sf::Vector2f GetFrameSize() const override;
	IPhotoFrame::Ptr MakeDefaultFrame() const {
		return IPhotoFrame::Ptr(new BasicPhotoFrame(GetFrameSize()));
	}

private:
	int m_width, m_height, m_numCols, m_numRows;
	float m_xScale, m_yScale;

	std::vector<int> m_fillOrder;
};

class SimpleTomato : public IFlyingObject {
public:
	SimpleTomato(const Scene::WeakPtr& scene);

	void OnCollision(bool* destroy) override;
	sf::FloatRect GetGlobalBounds() const override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
	Scene::WeakPtr m_scene;
	sf::CircleShape m_shape;
};

class SimpleTomatoSplash : public ISplashObject {
public:
	using Ptr = std::shared_ptr<SimpleTomatoSplash>;

	SimpleTomatoSplash(const sf::Vector2f& pos);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::CircleShape m_shape;
};