#include "SceneLayoutImpls.h"

void BasicPhotoFrame::SetSize(const sf::Vector2f& size)
{
	m_shape.setSize(size);
	m_shape.setFillColor(sf::Color::Transparent);
	m_shape.setOutlineColor(sf::Color::Black);
	m_shape.setOutlineThickness(5.0);
	m_shape.setOrigin(size.x / 2, size.y / 2);
}

sf::Vector2f BasicPhotoFrame::GetVisibleSize() const
{
	return m_shape.getSize();
}

void BasicPhotoFrame::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_shape, states);
}

TexturePhotoFrame::TexturePhotoFrame(const std::string& filename,
									 const sf::Vector2f& frameVisibleSize,
									 const sf::Vector2f& frameScale)
{
	m_txt.loadFromFile(filename);
	m_sprite.setTexture(m_txt, true);
	
	const auto& rect = m_sprite.getLocalBounds();
	m_sprite.setOrigin(rect.width / 2, rect.height / 2);
	m_sprite.setScale(frameScale);

	m_visibleSize.x = frameVisibleSize.x * frameScale.x;
	m_visibleSize.y = frameVisibleSize.y * frameScale.y;
}

sf::Vector2f TexturePhotoFrame::GetVisibleSize() const
{
	return m_visibleSize;
}

void TexturePhotoFrame::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_sprite, states);
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

	sf::Vector2f offsets;
	offsets.x = m_width / (m_numCols);
	offsets.y = m_height / (m_numRows);

	int col_idx = i % m_numCols;
	int row_idx = i / m_numCols;

	sf::Vector2f ret;

	ret.x = (col_idx + 0.5) * offsets.x;
	ret.y = (row_idx + 0.5) * offsets.y;

	return ret;
}

sf::Vector2f BasicGridLayout::GetFrameSize() const {
	sf::Vector2f ret;

	ret.x = m_width / m_numCols * m_xScale;
	ret.y = m_height / m_numRows * m_yScale;

	return ret;
}

SimpleTomato::SimpleTomato(const Scene::WeakPtr& scene)
	: m_scene(scene) {
	m_shape.setFillColor(sf::Color::Red);
	m_shape.setRadius(5.0);

	auto bounds = m_shape.getGlobalBounds();
	m_shape.setOrigin(bounds.width / 2, bounds.height / 2);
}

void SimpleTomato::OnCollision(bool* destroy) {
	// remove this tomato from scene and add splash
	auto scene = m_scene.lock();

	if (!scene) {
		return;
	}

	*destroy = true;

	SimpleTomatoSplash::Ptr splash = SimpleTomatoSplash::Ptr(new SimpleTomatoSplash(getPosition()));
	scene->AddSplashObject(splash);
}

sf::FloatRect SimpleTomato::GetGlobalBounds() const {
	return getTransform().transformRect(m_shape.getGlobalBounds());
}

void SimpleTomato::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(m_shape, states);
}


SimpleTomatoSplash::SimpleTomatoSplash(const sf::Vector2f& pos) {
	setPosition(pos);

	m_shape.setRadius(15.0);
	m_shape.setFillColor(sf::Color(255, 0, 0, 64));

	auto bounds = m_shape.getGlobalBounds();
	m_shape.setOrigin(bounds.width / 2, bounds.height / 2);
}

void SimpleTomatoSplash::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(m_shape, states);
}