#include "Boid.h"
#include "Quadtree.h"
#include "DiskGraph.h"

int   Boid::m_boidCount = 0;
float Boid::m_separationWeight;
float Boid::m_cohesionWeight;
float Boid::m_allignmentWeight;

Boid::Boid(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f velocity, sf::Vector2f& dimension, int detectionRadius) :
	m_window(window),
	m_dimension(dimension),
	m_position(position), m_velocity(velocity),
	m_detectionRadius(detectionRadius), m_detectionAngle(sf::degrees(270)),
	m_boidsInRange(),
	m_maxSpeed(MAX_SPEED), m_maxSteer(STEER_MULTIPLIER),
	m_id(m_boidCount++),
	m_boidTexture("resources/boid.png"),
	m_boidSprite(m_boidTexture)
{
	m_boidSprite.setColor(sf::Color(0, 128, 255));

	m_boidSprite.setScale(sf::Vector2f(0.5f, 0.5f));
	m_boidSprite.setPosition(m_position);

	sf::FloatRect bounds = m_boidSprite.getLocalBounds();
	m_boidSprite.setOrigin(sf::Vector2f(bounds.size.x / 2, bounds.size.y / 2));
}

void Boid::setSeparationWeight(float weight)
{
	m_separationWeight = weight;
}

void Boid::setCohesionWeight(float weight)
{
	m_cohesionWeight = weight;
}

void Boid::setAllignmentWeight(float weight)
{
	m_allignmentWeight = weight;
}

sf::Vector2f Boid::getPoint() const
{
	return m_position;
}

float Boid::getRadius() const
{
	return m_detectionRadius;
}

void Boid::draw(bool debug, float deltaTime)
{
	m_updatePosition(deltaTime);
	m_window.draw(m_boidSprite);

	if (debug) debugDraw();
}

void Boid::debugDraw()
{
	bool cone(true);
	bool line(true);
	bool dot(false);
	bool velocity(false);
	bool steer(true);

	sf::Vertex vertex;
	if (cone) //LoS cone
	{
		sf::VertexArray detectionCone(sf::PrimitiveType::LineStrip);
		sf::Angle rotation(sf::radians(m_boidSprite.getRotation().asRadians()));

		vertex.position = sf::Vector2f(m_position.x, m_position.y);
		detectionCone.append(vertex);
		float detectionAngleDegrees = m_detectionAngle.asDegrees();
		for (float i = -detectionAngleDegrees / 2.f; i <= detectionAngleDegrees / 2.f; i += detectionAngleDegrees / 10.f)
		{
			sf::Angle angle = sf::Angle(sf::degrees(i));
			vertex.position = float(m_detectionRadius) * m_velocity.normalized().rotatedBy(angle) + m_position;
			detectionCone.append(vertex);
		}
		vertex.position = sf::Vector2f(m_position.x, m_position.y);
		detectionCone.append(sf::Vertex(vertex));
		m_window.draw(detectionCone);
	}

	if (line) //Line from boid to every boid it sees
	{
		for (auto boid : m_boidsInRange)
		{
			sf::VertexArray line(sf::PrimitiveType::Lines);
			sf::Vertex vertex;
			vertex.position = sf::Vector2f(m_position.x, m_position.y);
			vertex.color = sf::Color::Red;
			line.append(vertex);

			vertex.position = sf::Vector2f(
				boid->m_position.x,
				boid->m_position.y);
			vertex.color = sf::Color::Red;
			line.append(vertex);
			m_window.draw(line);
		}
	}

	//if (dot) //Lines inside cone using dot product
	//{
	//	sf::Angle rotation(sf::radians(m_boidSprite.getRotation().asRadians()));

	//	sf::Angle angle1(sf::radians(m_detectionCone / 2 - M_PI_2));
	//	sf::Angle angle2(sf::radians(-m_detectionCone / 2 - M_PI_2));
	//	sf::Vector2f detectionPointOne(std::cos((angle1 + rotation).asRadians()),
	//		std::sin((angle2 + rotation).asRadians()));

	//	sf::Vector2f detectionPointTwo(std::cos((angle2 + rotation).asRadians()),
	//		std::sin((angle2 + rotation).asRadians()));

	//	sf::Vector2f detectionPointOneReciprocal(-reciprocal(-detectionPointOne).x, -reciprocal(detectionPointOne).y);
	//	sf::Vector2f detectionPointTwoReciprocal(-reciprocal(detectionPointTwo).x, -reciprocal(-detectionPointTwo).y);

	//	sf::VertexArray insideDetection(sf::PrimitiveType::LineStrip);

	//	vertex.position = sf::Vector2f(m_position.x, m_position.y);
	//	vertex.color = sf::Color::Transparent;
	//	insideDetection.append(vertex);
	//	for (float i = 0.f; i < 2.f * M_PI; i += 0.1f)
	//	{
	//		float x = std::cos(i);
	//		float y = std::sin(i);

	//		bool dotOne = detectionPointOneReciprocal.x * x + detectionPointOneReciprocal.y * y > 0;
	//		bool dotTwo = detectionPointTwoReciprocal.x * x + detectionPointTwoReciprocal.y * y > 0;
	//		bool inCone = m_detectionCone > M_PI ? dotOne || dotTwo : dotOne && dotTwo;

	//		if (inCone)
	//		{
	//			vertex.position = sf::Vector2f(
	//				m_position.x + x * m_detectionRadius,
	//				m_position.y + y * m_detectionRadius);
	//			vertex.color = sf::Color::Green;
	//			insideDetection.append(vertex);

	//			vertex.position = sf::Vector2f(
	//				m_position.x,
	//				m_position.y);
	//			vertex.color = sf::Color::Transparent;
	//			insideDetection.append(vertex);
	//		}
	//	}
	//	vertex.position = sf::Vector2f(
	//		m_position.x,
	//		m_position.y);
	//	vertex.color = sf::Color::Green;
	//	insideDetection.append(vertex);
	//	m_window.draw(insideDetection);
	//}

	if (velocity) //boid velocity
	{
		sf::Vertex velocity[] =
		{
			sf::Vertex({ {m_position}, {sf::Color::Red} }),
			sf::Vertex({ {m_position + m_velocity}, {sf::Color::Red} })
		};
		m_window.draw(velocity, 2, sf::PrimitiveType::Lines);
	}
}

void Boid::m_updatePosition(float deltaTime)
{
	m_position += m_velocity * deltaTime;

	if (m_position.x < 0) m_position.x += m_dimension.x;
	else if (m_position.x > m_dimension.x) m_position.x -= m_dimension.x;

	if (m_position.y < 0) m_position.y += m_dimension.y;
	else if (m_position.y > m_dimension.y) m_position.y -= m_dimension.y;

	m_boidSprite.setPosition(m_position);
	m_boidSprite.setRotation(sf::Angle(sf::radians(std::atan2f(m_velocity.y, m_velocity.x) + M_PI_2)));
}

void Boid::updateVelocity(Quadtree& quadtree, const std::vector<std::shared_ptr<Boid>>& boids, float deltaTime)
{
	m_maxSpeed = MAX_SPEED * deltaTime;
	//m_maxSteer = MAX_STEER * deltaTime;

	m_saveInCone(quadtree, boids);

	sf::Vector2f steer = m_separation(boids) + m_cohesion(boids) + m_allignment(boids);
	steer = m_separation(boids);

	m_velocity += steer * m_maxSteer;

	//m_velocity = normalise(m_velocity) * MAX_SPEED;
	m_velocity = limit(m_velocity, MAX_SPEED);
}

void Boid::updateVelocity(DiskGraph& diskGraph, const std::vector<std::shared_ptr<Boid>>& boids, float deltaTime)
{
	m_saveInCone(diskGraph, boids);

	sf::Vector2f separation(m_separation(boids));
	sf::Vector2f cohesion(m_cohesion(boids));
	sf::Vector2f allignment(m_allignment(boids));
	sf::Vector2f steer = separation + cohesion + allignment;

	m_velocity += steer * STEER_MULTIPLIER * deltaTime;
	m_velocity += m_velocity.normalized() * ACCELERATION * deltaTime;

	m_velocity = limit(m_velocity, MAX_SPEED);
}

sf::Vector2f Boid::m_separation(const std::vector<std::shared_ptr<Boid>>& boids)
{
	if (m_boidsInRange.size() == 0) return { 0, 0 };

	sf::Vector2f separationVelocity;

	for (auto boid : m_boidsInRange)
	{
		sf::Vector2f boidToPosition(m_position - boid->m_position);

		// The closer the boids are, the more they repel each other
		separationVelocity += boidToPosition.normalized() * (m_detectionRadius - boidToPosition.length());
	}
	separationVelocity /= float(m_boidsInRange.size());

	return separationVelocity * m_separationWeight;
}

sf::Vector2f Boid::m_cohesion(const std::vector<std::shared_ptr<Boid>>& boids)
{
	if (m_boidsInRange.size() == 0) return { 0, 0 };

	sf::Vector2f cohesionVelocity;

	for (auto boid : m_boidsInRange)
	{
		cohesionVelocity += boid->getPoint();
	}
	cohesionVelocity /= float(m_boidsInRange.size());
	cohesionVelocity -= m_position;

	return cohesionVelocity * m_cohesionWeight;
}

sf::Vector2f Boid::m_allignment(const std::vector<std::shared_ptr<Boid>>& boids)
{
	if (m_boidsInRange.size() == 0) return { 0, 0 };

	sf::Vector2f allignmentVelocity(m_detectionRadius / 2.f, 0.f);
	sf::Angle averageAngle;

	for (auto boid : m_boidsInRange)
	{
		averageAngle += sf::Vector2f({ 1.f, 0.f }).angleTo(boid->m_velocity);
	}

	averageAngle /= float(m_boidsInRange.size());

	sf::Angle currentAngle = sf::Vector2f({ 1.f, 0.f }).angleTo(m_velocity);

	allignmentVelocity = (allignmentVelocity.rotatedBy(averageAngle) - allignmentVelocity.rotatedBy(currentAngle)) * m_allignmentWeight;

	return allignmentVelocity * m_allignmentWeight;
}

bool Boid::m_inCone(const std::shared_ptr<Boid> boid)
{
	if ((boid->getPoint() - m_position).length() > m_detectionRadius) return false;
	return abs(m_velocity.angleTo(boid->getPoint() - m_position).asRadians()) <= m_detectionAngle.asRadians() / 2.0f;
}

void Boid::m_saveInCone(Quadtree& quadtree, const std::vector<std::shared_ptr<Boid>>& boids)
{
	m_boidsInRange.clear();
	//sf::FloatRect area({ m_position.x - m_detectionRadius, m_position.y - m_detectionRadius },
	//	{ m_detectionRadius * 2.f, m_detectionRadius * 2.f });

	std::vector<std::shared_ptr<Boid>> possibleInRange = quadtree.search(m_position);

	for (auto boid : possibleInRange)
	{
		sf::Vector2f diff(m_position - boid->m_position);
		float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

		if (boid->getPoint() != getPoint() && distance < m_detectionRadius && m_inCone(boid))
			m_boidsInRange.push_back(boid);
	}
}

void Boid::m_saveInCone(DiskGraph& diskGraph, const std::vector<std::shared_ptr<Boid>>& boids)
{
	m_boidsInRange.clear();
	//sf::FloatRect area({ m_position.x - m_detectionRadius, m_position.y - m_detectionRadius },
	//	{ m_detectionRadius * 2.f, m_detectionRadius * 2.f });

	std::vector<std::shared_ptr<Boid>> possibleInRange = diskGraph.search(m_position);

	for (auto boid : possibleInRange)
	{
		sf::Vector2f diff(m_position - boid->m_position);

		if (boid->getID() != m_id && diff.length() <= m_detectionRadius && m_inCone(boid))
			m_boidsInRange.push_back(boid);
	}
}

int Boid::getID()
{
	return m_id;
}

void Boid::m_clamp(float& val, int min, int max)
{
	if (val >= max)
		val = (float)max;
	else if (val <= min)
		val = (float)min;
}