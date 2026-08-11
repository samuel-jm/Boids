#pragma once

#include "Boid.h"
#include "Quadtree.h"
#include "Slider.h"

#include <iostream>
#include <random>

const bool DEBUG = false;

class Boids
{
public:
	Boids();
	~Boids();

	void run();

private:
	sf::RenderWindow m_window;

	sf::Clock m_clock;

	std::vector<std::shared_ptr<Boid>> m_boids;

	sf::Vector2f* m_dimension;

	sf::Font m_font;
	sf::Text m_sepText;
	sf::Text m_cohText;
	sf::Text m_allText;

	Slider m_sep;
	Slider m_coh;
	Slider m_all;

	Quadtree m_quadtree;

	float* m_sepWeight;
	float* m_cohWeight;
	float* m_allWeight;

	float m_deltaTime;

	void m_draw();
	void m_drawText();
	void m_updateVelocity(float deltaTime);
	void m_updateTree();
};