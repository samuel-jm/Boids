#include "Boid.h"
#include "BoidType.h"

int   Boid::m_boidCount = 0;
float Boid::m_separationWeight;
float Boid::m_cohesionWeight;
float Boid::m_allignmentWeight;

Boid::Boid(std::shared_ptr<BoidType> boidType, sf::Vector2f position, sf::Vector2f velocity, int detectionRadius) :
	boidType(boidType),
	m_position(position), m_velocity(velocity),
	m_boidsInRange(),
	m_id(m_boidCount++)
{}