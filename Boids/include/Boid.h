#pragma once

#define _USE_MATH_DEFINES

#include "SFML/Graphics.hpp"

#include <iostream>
#include <algorithm>

const float MAX_SPEED = 150.f;
const float MAX_STEER = 2.f;

class Quadtree;
class Boid
{
public:
	Boid(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f velocity, sf::Vector2f& dimension, float* sepWeight, float* cohWeight, float* allWeight, int detectionRadius, int m_id);

	sf::Vector2f getPoint() const;
	float getRadius() const;

	int getID();

	void updateVelocity(Quadtree& quadtree, const std::vector<std::shared_ptr<Boid>>&, float deltaTime);
	void draw(bool debug, float deltaTime);
	void debugDraw();

private:
	sf::RenderWindow& m_window;

	sf::Texture m_boidTexture;
	sf::Sprite m_boidSprite;

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;

	sf::Vector2f& m_dimension;

	int m_id;

	int m_detectionRadius;
	float m_detectionCone;

	float m_maxSpeed;
	float m_maxSteer;

	float* m_sepWeight; //TODO: slider, desired seperation
	float* m_allWeight; //TODO: slider, allignment range
	float* m_cohWeight; //TODO: slider, 0.01 - 0.05

	std::vector<std::shared_ptr<Boid>> m_boidsInRange;

	void m_updatePosition(float deltaTime);

	sf::Vector2f m_seperation(const std::vector<std::shared_ptr<Boid>>& boids);
	sf::Vector2f m_allignment(const std::vector<std::shared_ptr<Boid>>& boids);
	sf::Vector2f m_cohesion(const std::vector<std::shared_ptr<Boid>>& boids);

	bool m_inCone(const std::shared_ptr<Boid> boid);
	void m_saveInCone(Quadtree& quadtree, const std::vector<std::shared_ptr<Boid>>& boids);

	inline void m_clamp(float& pos, int min, int max);
};

template <class T> int sign(T val)
{
	return val < 0 ? -1 : 1;
}

template <class T> sf::Vector2<T> reciprocal(sf::Vector2<T> val)
{
	return sf::Vector2<T>(val.y, val.x);
}


template <class T> sf::Vector2<T> limit(sf::Vector2<T> vec, T limit)
{
	T mag = std::sqrt(vec.x * vec.x + vec.y * vec.y);

	T ratio = limit / mag;
	if (ratio < 1)
	{
		vec.x *= ratio;
		vec.y *= ratio;
	}

	return vec;
}

template <class T> sf::Vector2<T> normalise(sf::Vector2<T> vec)
{
	T mag = std::sqrt(vec.x * vec.x + vec.y * vec.y);

	if (mag == 0) return vec;

	T ratio = (T)1 / mag;

	vec.x *= ratio;
	vec.y *= ratio;

	return vec;
}