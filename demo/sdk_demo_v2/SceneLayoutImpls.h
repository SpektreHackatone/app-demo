#pragma once

#include "SceneElements.h"

class BasicPhotoFrame : public IPhotoFrame {
public:
	BasicPhotoFrame(const sf::Vector2f& size) {
		SetSize(size);
	}

	void SetSize(const sf::Vector2f& size);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::RectangleShape m_shape;
};

class BasicGridLayout : public ILayout {
public:
	BasicGridLayout(int width, int height, int num_cols, int num_rows, float x_scale = 0.7, float y_scale = 0.75);

protected:
	sf::Vector2f GetPositionFor(int i) const override;
	sf::Vector2f GetFrameSize() const override;
	IPhotoFrame::Ptr MakeDefaultFrame() const {
		return IPhotoFrame::Ptr(new BasicPhotoFrame(GetFrameSize()));
	}

private:
	int m_width, m_height, m_numCols, m_numRows;
	float m_xScale, m_yScale;
};
