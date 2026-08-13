#include "Slider.h"

Slider::Slider(sf::IntRect area, int defaultValue) :
	m_value(defaultValue), m_min(area.position.x), m_max(area.position.x + area.size.x)
{
	m_barLight = sf::RectangleShape(sf::Vector2f(area.size.x / 2, area.size.y));
	m_barDark = sf::RectangleShape(sf::Vector2f(area.size.x, area.size.y));
	m_middle = sf::RectangleShape(sf::Vector2f(4, area.size.y));
	m_circle = sf::CircleShape(area.size.y);

	m_barLight.setPosition(sf::Vector2f(area.position.x, area.position.y));
	m_barDark.setPosition(sf::Vector2f(area.position.x, area.position.y));
	m_middle.setPosition(sf::Vector2f(area.position.x + area.size.x / 2 - 2, area.position.y));
	m_circle.setPosition(sf::Vector2f(area.position.x + area.size.x / 2 - m_circle.getRadius(), area.position.y - area.size.y / 2));

	m_barLight.setFillColor(sf::Color(40, 121, 242));
	m_barDark.setFillColor(sf::Color(40, 121, 242, 127));
	m_middle.setFillColor(sf::Color::White);
	m_circle.setFillColor(sf::Color(4, 104, 255));
}

void Slider::setValue(int val)
{
	clamp(val, m_min, m_max);

	float percent = ((val - m_min) / ((m_max - m_min) / 10)) / 10.f;
	float position = percent * (m_max - m_min) + m_min;
	val = position - m_min;

	m_circle.setPosition(sf::Vector2f(position - m_circle.getRadius(), m_circle.getPosition().y));
	m_barLight.setSize(sf::Vector2f(val, m_barLight.getSize().y));

	m_value = val;
}

float Slider::getValue()
{
	return m_value;
}

sf::IntRect Slider::getArea()
{
	sf::Vector2f position = m_circle.getPosition();
	int radius = m_circle.getRadius();

	return sf::IntRect(sf::Vector2i(position.x, position.y), sf::Vector2i(radius * 2, radius * 2));
}

void Slider::setPosition(sf::Vector2f position)
{
}

void Slider::setScale(sf::Vector2f scale)
{
}

void Slider::draw(sf::RenderWindow& window)
{
	window.draw(m_barDark);
	window.draw(m_barLight);
	window.draw(m_middle);
	window.draw(m_circle);
}
