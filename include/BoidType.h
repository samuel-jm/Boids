#pragma once

#define _USE_MATH_DEFINES

#include "SFML/Graphics.hpp"
#include "ITranslatable.h"
#include "IPartitioner.h"

#include <memory>

class Boid;

/// <summary>
/// This class contains the intrinsic data for Boid instances and is used as part of the Flywieght pattern.
/// It contains data such as the texture and sprite which is shared among all boids.
/// </summary>
class BoidType
{
public:
	/// <summary>
	/// BoidType constructor
	/// </summary>
	/// <param name="texturePath">The path to the boid texture</param>
	/// <param name="detectionRadius">Detection radius used by all boids of this type</param>
	/// <param name="detectionAngle">Detection angle used by all boids of this type</param>
	BoidType(const char* texturePath, int detectionRadius, const sf::Angle& detectionAngle);

	static void setSeparationWeight(float weight);
	static void setCohesionWeight(float weight);
	static void setAllignmentWeight(float weight);

	int getID(std::shared_ptr<Boid> boid) const;
	void updateVelocity(std::shared_ptr<Boid> boid, std::unique_ptr<IPartitioner<sf::Vector2f>>& quadtree, const std::vector<std::shared_ptr<Boid>>& boids, float deltaTime);
	void updatePosition(std::shared_ptr<Boid> boid, float deltaTime, const sf::Vector2f& windowSize);

	float getRadius(std::shared_ptr<Boid> boid) const;
	void draw(std::shared_ptr<Boid> boid, sf::RenderWindow& window, bool debug);
	void debugDraw(std::shared_ptr<Boid> boid, sf::RenderWindow& window);

private:
	sf::Texture m_boidTexture;
	sf::Sprite m_boidSprite;

	/// <summary>
	/// The angle within which the boid can detect other boids, with the potential blind-spot being behind the boid
	/// </summary>
	sf::Angle m_detectionAngle;

	int m_detectionRadius;

	/// <summary>
	/// Returns a vector describing how this boid wants to be separated from other the other boids it can see
	/// </summary>
	/// <returns>A vector pointing away from the boids it can see</returns>
	sf::Vector2f m_separation(std::shared_ptr<Boid> boid);

	/// <summary>
	/// Returns a vector describing how this boid wants its direction vector to be alligned with
	/// the average direction vector of the boids it can see
	/// </summary>
	/// <returns>A vector pointing in the average direction of the boids it can see</returns>
	sf::Vector2f m_allignment(std::shared_ptr<Boid> boid);

	/// <summary>
	/// Returns a vector describing how this boid wants to be positioned in the average position of the boids it can see
	/// </summary>
	/// <returns>A vector pointing toward the average position of the boids it can see</returns>
	sf::Vector2f m_cohesion(std::shared_ptr<Boid> boid);

	/// <summary>
	/// This finds the boids that are nearby to this boid through the IPartitioner and determines which of those it can
	/// actually see, and saves them in m_boidsInRange
	/// </summary>
	/// <param name="partitioner">The partitioner this boid uses to search for nearby boids</param>
	void m_saveInCone(std::shared_ptr<Boid> boid, std::unique_ptr<IPartitioner<sf::Vector2f>>& partitioner);
};

