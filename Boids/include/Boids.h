#pragma once

#include "Boid.h"
#include "Quadtree.h"
#include "Slider.inl"
#include "DiskGraph.h"

#include <iostream>
#include <random>

const bool DEBUG = true;

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
	sf::Text m_separationText;
	sf::Text m_cohesionText;
	sf::Text m_allignmentText;
	sf::Text m_fpsText;

	Slider<float> m_separation;
	Slider<float> m_cohesion;
	Slider<float> m_allignment;

	Quadtree<sf::Vector2f> m_quadtree;
	DiskGraph m_diskGraph;

	float m_deltaTime;

	void m_draw();
	void m_drawText();
	void m_updateBoids(float deltaTime);
	void m_updateBoidsVelocities(float deltaTime);
	void m_updateBoidsPositions(float deltaTime);
	void m_resetTree();
};