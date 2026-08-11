#pragma once

#include "SFML/Graphics.hpp"

#include <iostream>

class Slider
{
public:
	Slider(sf::IntRect area);

	void setValue(int val);
	float getValue();

	sf::IntRect getArea();

	void setPosition(sf::Vector2f position);
	void setScale(sf::Vector2f scale);

	void draw(sf::RenderWindow& window);

private:

	sf::RectangleShape m_barLight;
	sf::RectangleShape m_barDark;
	sf::RectangleShape m_middle;
	sf::CircleShape m_circle;

	float m_value; //percentage
	const int m_min;
	const int m_max;
};

template<class T> void clamp(T& val, T min, T max)
{
	if (val > max)
		val = max;
	else if (val < min)
		val = min;
}

