#pragma once

#include "SFML/Graphics.hpp"

#include <iostream>

/// <summary>
/// This class represents a UI slider element for the given type T (float, int, etc.)
/// </summary>
/// <typeparam name="T">The type of value the slider ranges between (float, int, etc.)</typeparam>
template<typename T>
class Slider
{
public:
	/// <summary>
	/// This constructor sets the position and size of the slider in the window it is drawn in,
	/// as well as the min, max, and initial values for the slider
	/// </summary>
	/// <param name="topLeft">The top-left position of the slider in pixels</param>
	/// <param name="size">The width and height of the slider in pixels</param>
	/// <param name="minValue">The minumum value the slider will go to</param>
	/// <param name="maxValue">The maxumum value the slider will go to</param>
	/// <param name="initialValue">The initial vallue the slider is set to</param>
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

	/// <summary>
	/// Determines whether the given mouse position is overlapping the slider
	/// </summary>
	/// <param name="mousePos">The position of the mouse, relative to the window the slider is drawn in</param>
	/// <returns>Wether or not the mouse is hovering over the slider</returns>
	bool mouseOver(sf::Vector2i mousePos)
	{
		sf::Vector2f topLeft = static_cast<sf::Vector2f>(m_topLeft);
		sf::Vector2f size = static_cast<sf::Vector2f>(m_size);
		return sf::FloatRect(topLeft, size).contains(sf::Vector2f(mousePos));
	}

	/// <summary>
	/// Updates the slider's value according to the given mouse position
	/// </summary>
	/// <param name="mousePos">The position of the mouse, relative to the window the slider is drawn in</param>
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

	void setTopLeft(sf::Vector2u topLeft)
	{
		sf::Vector2u currTopLeft = m_topLeft;
		sf::Vector2f diff = topLeft - m_topLeft;
		m_topLeft = topLeft;

		m_barLight.setPosition(m_barLight.getPosition() + diff);
		m_barDark.setPosition(m_barDark.getPosition() + diff);
		m_middle.setPosition(m_middle.getPosition() + diff);
		m_circle.setPosition(m_circle.getPosition() + diff);
	}

	sf::Vector2u getTopLeft() { return m_topLeft; }

	/// <summary>
	/// Renders the slider to the input window
	/// </summary>
	/// <param name="window">The window to render the slider to</param>
	void draw(sf::RenderWindow& window)
	{
		window.draw(m_barDark);
		window.draw(m_barLight);
		window.draw(m_middle);
		window.draw(m_circle);
	}

private:
	sf::Vector2u m_topLeft;
	sf::Vector2u m_size;
	sf::RectangleShape m_barLight;
	sf::RectangleShape m_barDark;
	sf::RectangleShape m_middle;
	sf::CircleShape m_circle;

	T m_value;
	const T m_minValue;
	const T m_maxValue;
	const int m_middleBarWidth;
	const int m_circleRadius;

	/// <summary>
	/// Returns how close the slider's value is to the max value, as a percentage
	/// </summary>
	/// <returns>The distance the slider's value is to the max value, as a percentage</returns>
	float m_valuePercent()
	{
		return float(m_value - m_minValue) / float(m_maxValue - m_minValue);
	}

	/// <summary>
	/// Clamps the given value to the given min and max values
	/// </summary>
	/// <param name="val">The value to clamp</param>
	/// <param name="min">The minimum value</param>
	/// <param name="max">The maximum value</param>
	void clamp(T& val, T min, T max)
	{
		if (val > max)
			val = max;
		else if (val < min)
			val = min;
	}
};