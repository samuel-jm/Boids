#include "BoidType.h"
#include "Boid.h"

BoidType::BoidType(const char* texturePath, int detectionRadius, const sf::Angle& detectionAngle) :
	m_detectionRadius(detectionRadius), m_detectionAngle(detectionAngle),
	m_boidTexture("resources/boid.png"),
	m_boidSprite(m_boidTexture)
{
	m_boidSprite.setColor(sf::Color(0, 128, 255));

	m_boidSprite.setScale(sf::Vector2f(0.5f, 0.5f));

	sf::FloatRect bounds = m_boidSprite.getLocalBounds();
	m_boidSprite.setOrigin(sf::Vector2f(bounds.size.x / 2, bounds.size.y / 2));
}

void BoidType::setSeparationWeight(float weight)
{
	Boid::m_separationWeight = weight;
}

void BoidType::setCohesionWeight(float weight)
{
	Boid::m_cohesionWeight = weight;
}

void BoidType::setAllignmentWeight(float weight)
{
	Boid::m_allignmentWeight = weight;
}

void BoidType::draw(std::shared_ptr<Boid> boid, sf::RenderWindow& window, bool debug)
{
	m_boidSprite.setPosition(boid->getTranslation());
	m_boidSprite.setRotation(sf::Vector2f(0.f, -1.f).angleTo(boid->m_velocity));
	window.draw(m_boidSprite);

	if (debug) debugDraw(boid, window);
}

int BoidType::getID(const std::shared_ptr<Boid> boid) const { return boid->m_id; }

float BoidType::getRadius(std::shared_ptr<Boid> boid) const { return m_detectionRadius; }

void BoidType::debugDraw(std::shared_ptr<Boid> boid, sf::RenderWindow& window)
{
	static bool text(false);
	static bool cone(false);
	static bool line(false);
	static bool dot(false);
	static bool velocity(false);
	static bool steer(false);

	if (text)
	{
		static sf::Font f("fonts/arial.ttf");
		static sf::Text t(f);
		t.setString(std::to_string(boid->m_id));
		t.setPosition(boid->m_position);
		t.setCharacterSize(18);

		window.draw(t);
	}

	if (cone) //LoS cone
	{
		sf::Vertex vertex;
		sf::VertexArray detectionCone(sf::PrimitiveType::LineStrip);
		sf::Angle rotation(sf::radians(m_boidSprite.getRotation().asRadians()));

		vertex.position = sf::Vector2f(boid->m_position.x, boid->m_position.y);
		detectionCone.append(vertex);
		float detectionAngleDegrees = m_detectionAngle.asDegrees();
		for (float i = -detectionAngleDegrees / 2.f; i <= detectionAngleDegrees / 2.f; i += detectionAngleDegrees / 10.f)
		{
			sf::Angle angle = sf::Angle(sf::degrees(i));
			vertex.position = float(m_detectionRadius) * boid->m_velocity.normalized().rotatedBy(angle) + boid->m_position;
			detectionCone.append(vertex);
		}
		vertex.position = sf::Vector2f(boid->m_position.x, boid->m_position.y);
		detectionCone.append(sf::Vertex(vertex));
		window.draw(detectionCone);
	}

	if (line) //Line from boid to every boid it sees
	{
		for (auto boidInRange : boid->m_boidsInRange)
		{
			sf::VertexArray line(sf::PrimitiveType::Lines);
			sf::Vertex vertex;
			vertex.position = sf::Vector2f(boid->m_position.x, boid->m_position.y);
			vertex.color = sf::Color::Transparent;
			line.append(vertex);

			vertex.position = sf::Vector2f(
				boidInRange->m_position.x,
				boidInRange->m_position.y);
			vertex.color = sf::Color::Red;
			line.append(vertex);
			window.draw(line);
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
	//	window.draw(insideDetection);
	//}

	if (velocity) //boid velocity
	{
		sf::Vertex velocity[] =
		{
			sf::Vertex({ {boid->m_position}, {sf::Color::Red} }),
			sf::Vertex({ {boid->m_position + boid->m_velocity}, {sf::Color::Red} })
		};
		window.draw(velocity, 2, sf::PrimitiveType::Lines);
	}

	if (steer) //boid velocity
	{
		sf::Vertex velocity[] =
		{
			sf::Vertex({ {boid->m_position}, {sf::Color::Magenta} }),
			sf::Vertex({ {boid->m_position + boid->m_steer}, {sf::Color::Magenta} })
		};
		window.draw(velocity, 2, sf::PrimitiveType::Lines);

		if (boid->m_boidsInRange.size() == 0)
		{
			sf::Color tmp = m_boidSprite.getColor();
			m_boidSprite.setColor(sf::Color::Green);
			window.draw(m_boidSprite);
			m_boidSprite.setColor(tmp);
		}
	}
}

void BoidType::updateVelocity(std::shared_ptr<Boid> boid, std::unique_ptr<IPartitioner<sf::Vector2f>>& partitioner, const std::vector<std::shared_ptr<Boid>>& boids, float deltaTime)
{
	m_saveInCone(boid, partitioner);

	sf::Vector2f separation(m_separation(boid));
	sf::Vector2f cohesion(m_cohesion(boid));
	sf::Vector2f allignment(m_allignment(boid));
	sf::Vector2f steer = separation + cohesion + allignment;
	boid->m_steer = steer;

	boid->m_velocity += steer * STEER_MULTIPLIER * deltaTime;
	boid->m_velocity += boid->m_velocity.normalized() * ACCELERATION * deltaTime;

	boid->m_velocity = limit(boid->m_velocity, MAX_SPEED);
}

void BoidType::updatePosition(std::shared_ptr<Boid> boid, float deltaTime, const sf::Vector2f& windowSize)
{
	boid->m_position += boid->m_velocity * deltaTime;

	if (boid->m_position.x < 0) boid->m_position.x += windowSize.x;
	else if (boid->m_position.x > windowSize.x) boid->m_position.x -= windowSize.x;

	if (boid->m_position.y < 0) boid->m_position.y += windowSize.y;
	else if (boid->m_position.y > windowSize.y) boid->m_position.y -= windowSize.y;

	m_boidSprite.setPosition(boid->m_position);
	m_boidSprite.setRotation(sf::Angle(sf::radians(std::atan2f(boid->m_velocity.y, boid->m_velocity.x) + M_PI_2)));
}

sf::Vector2f BoidType::m_separation(std::shared_ptr<Boid> boid)
{
	if (boid->m_boidsInRange.size() == 0) return { 0, 0 };

	sf::Vector2f separationVelocity;

	for (auto boidInRange : boid->m_boidsInRange)
	{
		sf::Vector2f boidToPosition(boid->m_position - boidInRange->m_position);

		// The closer the boids are, the more they repel each other
		separationVelocity += boidToPosition.normalized() * (m_detectionRadius - boidToPosition.length());
	}
	separationVelocity /= float(boid->m_boidsInRange.size());

	return separationVelocity * boid->m_separationWeight;
}

sf::Vector2f BoidType::m_cohesion(std::shared_ptr<Boid> boid)
{
	if (boid->m_boidsInRange.size() == 0) return { 0, 0 };

	sf::Vector2f cohesionVelocity;

	for (auto boidInRange : boid->m_boidsInRange)
	{
		cohesionVelocity += boidInRange->getTranslation();
	}
	cohesionVelocity /= float(boid->m_boidsInRange.size());
	cohesionVelocity -= boid->m_position;

	return cohesionVelocity * boid->m_cohesionWeight;
}

sf::Vector2f BoidType::m_allignment(std::shared_ptr<Boid> boid)
{
	if (boid->m_boidsInRange.size() == 0) return { 0, 0 };

	sf::Vector2f velocity = boid->m_velocity;
	sf::Vector2f allignmentDirection;//(m_detectionRadius / 2.f, 0.f);
	sf::Angle averageAngle;

	for (auto boidInRange : boid->m_boidsInRange)
	{
		//averageAngle += sf::Vector2f({ 1.f, 0.f }).angleTo(boid->m_velocity);
		allignmentDirection += boidInRange->m_velocity;
	}
	allignmentDirection = allignmentDirection.normalized();

	sf::Angle rotation(sf::radians(velocity.angleTo(allignmentDirection).asRadians() * boid->m_allignmentWeight));

	velocity = velocity.rotatedBy(rotation);

	//averageAngle /= float(m_boidsInRange.size());

	//sf::Angle currentAngle = sf::Vector2f({ 1.f, 0.f }).angleTo(m_velocity);

	//std::cout << currentAngle.asDegrees() << std::endl;

	//allignmentVelocity = (allignmentVelocity.rotatedBy(averageAngle) - allignmentVelocity.rotatedBy(currentAngle)) * m_allignmentWeight;

	return (velocity - boid->m_velocity);
}

void BoidType::m_saveInCone(std::shared_ptr<Boid> boid, std::unique_ptr<IPartitioner<sf::Vector2f>>& partitioner)
{
	boid->m_boidsInRange.clear();
	//sf::FloatRect area({ m_position.x - m_detectionRadius, m_position.y - m_detectionRadius },
	//	{ m_detectionRadius * 2.f, m_detectionRadius * 2.f });

	auto possibleInRange = partitioner->search(boid);

	auto boidInSight = [&](const std::shared_ptr<Boid>& source, const std::shared_ptr<Boid>& target) 
		{ 
			return abs(source->m_velocity.angleTo(target->getTranslation() - source->m_position).asRadians()) <= m_detectionAngle.asRadians() / 2.0f; 
		};
	for (int i = 0; i < std::min(MAX_NEIGHBOURS_TO_CONSIDER, int(possibleInRange.size())); i++)
	{
		std::shared_ptr<Boid> boidInRange = std::dynamic_pointer_cast<Boid>(possibleInRange[i]);

		sf::Vector2f diff(boid->m_position - boidInRange->m_position);
		float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

		if (boidInRange->m_id != boid->m_id && distance > 0 && distance <= m_detectionRadius && boidInSight(boid, boidInRange))
			boid->m_boidsInRange.push_back(boidInRange);

	}
}