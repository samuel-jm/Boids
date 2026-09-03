#pragma once

#include "SFML/Graphics.hpp"
#include "Utilities.h"
#include "ITranslatable.h"

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

const int MAX_NEIGHBOURS_TO_CONSIDER = 50;

class BoidType;

/// <summary>
/// This class represents a Boid, it inherits from ITranslatable to ensure it can be partitioned within
/// an IPartitioner (Quadtree, DiskGraph, etc.). It is used as part of the Flyweight pattern and contains
/// extrinsic data; position, velocity, etc. which is unique to every boid.
/// </summary>
class Boid : public ITranslatable<sf::Vector2f>
{
	friend BoidType;

public:

	/// <summary>
	/// This constructor initialises a Boid including which intrinsic BoidType it references
	/// </summary>
	/// <param name="boidType">Instance which holds this boid's intrinsic data</param>
	/// <param name="position">Where this boid is placed</param>
	/// <param name="velocity">This boid's velocity</param>
	/// <param name="detectionRadius">The detection radius of the boid</param>
	Boid(std::shared_ptr<BoidType> boidType, sf::Vector2f position, sf::Vector2f velocity, int detectionRadius);

	const sf::Vector2f& getTranslation() override { return m_position; }
	void setTranslation(const sf::Vector2f& translation) override { m_position = translation; }

private:
	std::shared_ptr<BoidType> boidType;

	int m_id;

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
};
