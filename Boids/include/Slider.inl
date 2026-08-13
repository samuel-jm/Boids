#pragma once

#include "SFML/Graphics.hpp"

#include <iostream>

template<typename T>
class Slider
{
public:
	Slider(sf::Vector2f topLeft, sf::Vector2f size, T minValue, T maxValue, T initialValue) :
		m_topLeft(topLeft), m_size(size), m_minValue(minValue), m_maxValue(maxValue), m_value(initialValue),
		m_middleBarWidth(4), m_circleRadius(m_size.y)
	{
		float percent = m_valuePercent();

		m_barLight = sf::RectangleShape(sf::Vector2f(percent * m_size.x, m_size.y));
		m_barDark = sf::RectangleShape(sf::Vector2f(m_size.x, m_size.y));
		m_middle = sf::RectangleShape(sf::Vector2f(m_middleBarWidth, m_size.y));
		m_circle = sf::CircleShape(m_circleRadius);

		m_barLight.setPosition(topLeft);
		m_barDark.setPosition(topLeft);
		m_middle.setPosition(topLeft + sf::Vector2f(m_size.x / 2.f - m_middleBarWidth / 2.f, 0.f));
		m_circle.setPosition(topLeft + sf::Vector2f(percent * m_size.x - m_circleRadius, -m_circleRadius / 2.f));

		m_barLight.setFillColor(sf::Color(40, 121, 242));
		m_barDark.setFillColor(sf::Color(40, 121, 242, 127));
		m_middle.setFillColor(sf::Color::White);
		m_circle.setFillColor(sf::Color(4, 104, 255));
	}

	bool mouseOver(sf::Vector2i mousePos)
	{
		return sf::FloatRect(m_topLeft, m_size).contains(sf::Vector2f(mousePos));
	}

	void update(sf::Vector2i mousePos)
	{
		float percent = (mousePos.x - m_topLeft.x) / m_size.x;

		setValue(percent * m_maxValue);
	}

	void setValue(T val)
	{
		clamp(val, m_minValue, m_maxValue);

		float percent = m_valuePercent();

		m_circle.setPosition(sf::Vector2f(m_topLeft) + sf::Vector2f(percent * m_size.x - m_circleRadius, -m_circleRadius / 2.f));
		m_barLight.setSize({ percent * m_size.x, float(m_size.y) });

		m_value = val;
	}

	T getValue() { return m_value; }

	void setSize(sf::Vector2u size)
	{
		sf::Vector2f position = m_circle.getPosition();
		int radius = m_circle.getRadius();

		return sf::IntRect(sf::Vector2i(position.x, position.y), sf::Vector2i(radius * 2, radius * 2));
	}

	sf::IntRect getSize() { return m_size; }

	void setTopLeft(sf::Vector2u topLeft)
	{
		sf::Vector2u currTopLeft = m_topLeft;
		sf::Vector2u diff = topLeft - m_topLeft;
		m_topLeft = topLeft;

		m_barLight.setPosition(m_barLight.getPotition() + diff);
		m_barDark.setPosition(m_barDark.getPotition() + diff);
		m_middle.setPosition(m_middle.getPotition() + diff);
		m_circle.setPosition(m_circle.getPotition() + diff);
	}

	sf::Vector2u getTopLeft() { return m_topLeft; }

	void draw(sf::RenderWindow& window)
	{
		window.draw(m_barDark);
		window.draw(m_barLight);
		window.draw(m_middle);
		window.draw(m_circle);
	}

private:
	sf::Vector2f m_topLeft;
	sf::Vector2f m_size;
	sf::RectangleShape m_barLight;
	sf::RectangleShape m_barDark;
	sf::RectangleShape m_middle;
	sf::CircleShape m_circle;

	T m_value;
	const T m_minValue;
	const T m_maxValue;
	const int m_middleBarWidth;
	const int m_circleRadius;

	float m_valuePercent()
	{
		return float(m_value - m_minValue) / float(m_maxValue - m_minValue);
	}

	void clamp(T& val, T min, T max)
	{
		if (val > max)
			val = max;
		else if (val < min)
			val = min;
	}
};