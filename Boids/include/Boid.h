#pragma once

#define _USE_MATH_DEFINES

#include "SFML/Graphics.hpp"
#include "Utilities.inl"
#include "ITranslatable.h"

#include <iostream>
#include <algorithm>

const float MAX_SPEED = 150.f;
const float STEER_MULTIPLIER = 30.f;
const float ACCELERATION = 50.f;

template<typename T>
class Quadtree;
class DiskGraph;
class Boid : public ITranslatable<sf::Vector2f>, public std::enable_shared_from_this<Boid>
{
public:
	static void setSeparationWeight(float weight);
	static void setCohesionWeight(float weight);
	static void setAllignmentWeight(float weight);

	Boid(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f velocity, sf::Vector2f& dimension, int detectionRadius);

	const sf::Vector2f& getTranslation() override;
	void setTranslation(const sf::Vector2f& translation) override;

	float getRadius() const;
	int getID();
	void updateVelocity(Quadtree<sf::Vector2f>& quadtree, const std::vector<std::shared_ptr<Boid>>&, float deltaTime);
	void updateVelocity(DiskGraph& diskGraph, const std::vector<std::shared_ptr<Boid>>&, float deltaTime);
	void updatePosition(float deltaTime, const sf::Vector2f& windowSize);
	void draw(sf::RenderWindow& window, bool debug);
	void debugDraw(sf::RenderWindow& window);

private:
	static int m_boidCount;
	static float m_separationWeight; //TODO: slider, desired separation
	static float m_allignmentWeight; //TODO: slider, allignment range
	static float m_cohesionWeight; //TODO: slider, 0.01 - 0.05

	sf::Texture m_boidTexture;
	sf::Sprite m_boidSprite;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_steer;
	std::vector<std::shared_ptr<Boid>> m_boidsInRange;
	sf::Angle m_detectionAngle; // The angle within which the boid can detect other boids, with the potential blind-spot being behind the boid

	int m_id;
	int m_detectionRadius;
	float m_maxSpeed;
	float m_maxSteer;

	sf::Vector2f m_separation(const std::vector<std::shared_ptr<Boid>>& boids);
	sf::Vector2f m_allignment(const std::vector<std::shared_ptr<Boid>>& boids);
	sf::Vector2f m_cohesion(const std::vector<std::shared_ptr<Boid>>& boids);
	bool m_inCone(const std::shared_ptr<Boid> boid);
	void m_saveInCone(Quadtree<sf::Vector2f>& quadtree, const std::vector<std::shared_ptr<Boid>>& boids);
	void m_saveInCone(DiskGraph& diskGraph, const std::vector<std::shared_ptr<Boid>>& boids);
	inline void m_clamp(float& pos, int min, int max);
};