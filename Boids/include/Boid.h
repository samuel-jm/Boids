#pragma once

#define _USE_MATH_DEFINES

#include "SFML/Graphics.hpp"
#include "Utilities.h"
#include "ITranslatable.h"
#include "IPartitioner.h"

#include <iostream>
#include <algorithm>
#include <memory>

const float MAX_SPEED = 150.f;

/// <summary>
/// The final steer vector is multiplied by this to ensure more responsive boid movement
/// </summary>
const float STEER_MULTIPLIER = 30.f;

/// <summary>
/// This is used to ensure the boids don't end up stopped somewhere
/// </summary>
const float ACCELERATION = 50.f;

/// <summary>
/// This class represents a Boid, it inherits from ITranslatable to ensure it can be partitioned within
/// an IPartitioner (Quadtree, DiskGraph, etc.). It is responsible for its position, rendering, and how
/// its velocity vector is influenced by nearby Boids.
/// </summary>
class Boid : public ITranslatable<sf::Vector2f>, public std::enable_shared_from_this<Boid>
{
public:
	static void setSeparationWeight(float weight) { m_separationWeight = weight; }
	static void setCohesionWeight(float weight) { m_cohesionWeight = weight;}
	static void setAllignmentWeight(float weight) { m_allignmentWeight = weight;}

	Boid(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f velocity, sf::Vector2f& dimension, int detectionRadius);

	const sf::Vector2f& getTranslation() override { return m_position; }
	void setTranslation(const sf::Vector2f& translation) override { m_position = translation; }

	float getRadius() const { return m_detectionRadius; }
	int getID() const { return m_id; }
	void updateVelocity(std::unique_ptr<IPartitioner<sf::Vector2f>>& quadtree, const std::vector<std::shared_ptr<Boid>>& boids, float deltaTime);
	void updatePosition(float deltaTime, const sf::Vector2f& windowSize);
	void draw(sf::RenderWindow& window, bool debug);
	void debugDraw(sf::RenderWindow& window);

private:
	static int m_boidCount;

	/// <summary>
	/// Used to determine how much the boid's velocity is influenced by the separation coponent
	/// </summary>
	static float m_separationWeight;
	
	/// <summary>
	/// Used to determine how much the boid's velocity is influenced by the allignment coponent
	/// </summary>
	static float m_allignmentWeight;
	
	/// <summary>
	/// Used to determine how much the boid's velocity is influenced by the cohesion coponent
	/// </summary>
	static float m_cohesionWeight;

	sf::Texture m_boidTexture;
	sf::Sprite m_boidSprite;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;

	/// <summary>
	/// This is the vector the boid attempts to steer towards, it is the sum of the separation, cohesion, and allignment vectors
	/// </summary>
	sf::Vector2f m_steer;

	/// <summary>
	/// The list of boids within this boid's radius, not necessarily visible to this boid
	/// </summary>
	std::vector<std::shared_ptr<Boid>> m_boidsInRange;

	/// <summary>
	/// The angle within which the boid can detect other boids, with the potential blind-spot being behind the boid
	/// </summary>
	sf::Angle m_detectionAngle;

	int m_id;
	int m_detectionRadius;
	float m_maxSpeed;
	float m_maxSteer;

	/// <summary>
	/// Returns a vector describing how this boid wants to be separated from other the other boids it can see
	/// </summary>
	/// <returns>A vector pointing away from the boids it can see</returns>
	sf::Vector2f m_separation();

	/// <summary>
	/// Returns a vector describing how this boid wants its direction vector to be alligned with
	/// the average direction vector of the boids it can see
	/// </summary>
	/// <returns>A vector pointing in the average direction of the boids it can see</returns>
	sf::Vector2f m_allignment();
	
	/// <summary>
	/// Returns a vector describing how this boid wants to be positioned in the average position of the boids it can see
	/// </summary>
	/// <returns>A vector pointing toward the average position of the boids it can see</returns>
	sf::Vector2f m_cohesion();
	
	/// <summary>
	/// This finds the boids that are nearby to this boid through the IPartitioner and determines which of those it can
	/// actually see, and saves them in m_boidsInRange
	/// </summary>
	/// <param name="partitioner">The partitioner this boid uses to search for nearby boids</param>
	void m_saveInCone(std::unique_ptr<IPartitioner<sf::Vector2f>>& partitioner);
};