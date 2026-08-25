#pragma once

#include "Boid.h"
#include "BoidType.h"
#include "Slider.inl"
#include "PartitionerFactory.inl"

#include <iostream>
#include <random>
#include <memory>

/// <summary>
/// Debug flag used for debug rendering, will eventually be changable at runtime
/// </summary>
const bool DEBUG = true;

/// <summary>
/// This class represents the main application. It is responsible for setting everything up
/// including the boids, partitioner, rendering etc.
/// </summary>
class Boids
{
public:
	Boids();

	void run();

private:
	sf::RenderWindow m_window;

	/// <summary>
	/// Clock used for finding the time between frames (dt)
	/// </summary>
	sf::Clock m_clock;

	std::shared_ptr<BoidType> m_boidType;
	std::vector<std::shared_ptr<Boid>> m_boids;

	/// <summary>
	/// Main window size
	/// </summary>
	sf::Vector2f m_dimension;

	sf::Font m_font;
	sf::Text m_separationText;
	sf::Text m_cohesionText;
	sf::Text m_allignmentText;
	sf::Text m_fpsText;

	Slider<float> m_separation;
	Slider<float> m_cohesion;
	Slider<float> m_allignment;

	std::unique_ptr<IPartitioner<sf::Vector2f>> m_partitioner;

	/// <summary>
	/// This method renders everything to the window
	/// </summary>
	void m_draw();

	/// <summary>
	/// This method renders the text to the window, it is called in m_draw().
	/// </summary>
	void m_drawText();

	/// <summary>
	/// This method will update the boids in m_boids in preparation for rendering.
	/// </summary>
	/// <param name="deltaTime">The time between the last frame and this frame</param>
	void m_updateBoids(float deltaTime);

	/// <summary>
	/// This method updates the velocity vectors of the boids in m_boids.
	/// </summary>
	/// <param name="deltaTime"></param>
	void m_updateBoidsVelocities(float deltaTime);
	
	/// <summary>
	/// This method updates the position of the boids in m_boids according to their velocity vectors
	/// </summary>
	/// <param name="deltaTime">The time between the last frame and this frame</param>
	void m_updateBoidsPositions(float deltaTime);
};