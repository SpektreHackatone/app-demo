#include "SceneLayoutImpls.h"

void BasicPhotoFrame::SetSize(const sf::Vector2f& size) {
	m_shape.setSize(size);
	m_shape.setFillColor(sf::Color::Transparent);
	m_shape.setOutlineColor(sf::Color::Black);
	m_shape.setOutlineThickness(5.0);
}

void BasicPhotoFrame::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();

	target.draw(m_shape, states);
}

BasicGridLayout::BasicGridLayout(int width, int height, int num_cols, int num_rows, float x_scale, float y_scale)
	: m_width(width)
	, m_height(height)
	, m_numCols(num_cols)
	, m_numRows(num_rows)
	, m_xScale(x_scale)
	, m_yScale(y_scale) {}

sf::Vector2f BasicGridLayout::GetPositionFor(int i) const {
	if (m_fillOrder.size() > 0) {
		i = m_fillOrder[i];
	}

	sf::Vector2f frame_size = GetFrameSize();
	sf::Vector2f offsets;
	offsets.x = (m_width - m_numCols * frame_size.x) / m_numCols / 2;
	offsets.y = (m_height - m_numRows * frame_size.y) / m_numRows / 2;

	int col_idx = i % m_numCols;
	int row_idx = i / m_numCols;

	sf::Vector2f ret;

	ret.x = col_idx * frame_size.x + (2 * col_idx + 1) * offsets.x;
	ret.y = row_idx * frame_size.y + (2 * row_idx + 1) * offsets.y;

	return ret;
}

sf::Vector2f BasicGridLayout::GetFrameSize() const {
	sf::Vector2f ret;

	ret.x = m_width / m_numCols * m_xScale;
	ret.y = m_height / m_numRows * m_yScale;

	return ret;
}