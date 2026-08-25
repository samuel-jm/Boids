#include "Boids.h"

const float INITIAL_SEPARATION = 1.f;
const float INITIAL_COHESION = 0.3f;
const float INITIAL_ALLIGNMENT = 0.2f;

const int QUADTREE_MAX_ITEMS = 30;
const int QUADTREE_MAX_LEVELS = 3;

const int BOID_DETECTION_RADIUS = 30;
const sf::Angle BOID_DETECTION_ANGLE = sf::Angle(sf::degrees(270));

Boids::Boids() :
	m_dimension(1200, 1000),
	m_window(sf::VideoMode({ 1200, 1000 }), "Boids"),
	m_clock(),
	//m_quadtree(30, 3, {0, 0}, {(float)m_window.getSize().x, (float)m_window.getSize().y}),
	m_font("fonts/arial.ttf"),
	m_separationText(m_font), m_cohesionText(m_font), m_allignmentText(m_font),
	m_fpsText(m_font),
	m_separation({ 20, 20 }, { 200, 10 }, 0.f, 1.f, INITIAL_SEPARATION),
	m_cohesion({ 240, 20 }, { 200, 10 }, 0.f, 0.5f, INITIAL_COHESION),
	m_allignment({ 460, 20 }, { 200, 10 }, 0.f, 1.f, INITIAL_ALLIGNMENT)
{
	sf::Vector2f position({ 0, 0 });
	sf::Vector2f size({ (float)m_window.getSize().x, (float)m_window.getSize().y });

	// Comment one and uncomment the other to try a different partitioner
	//m_partitioner = std::move(PartitionerFactory::createPartitioner<sf::Vector2f>("quadtree", QUADTREE_MAX_ITEMS, QUADTREE_MAX_LEVELS, position, size));
	m_partitioner = std::move(PartitionerFactory::createPartitioner<sf::Vector2f>("diskgraph", BOID_DETECTION_RADIUS, position, size));

	if (m_partitioner == nullptr)
	{
		std::cerr << "Please ensure the arguments for the given IPartitioner string match its constructor" << std::endl;
		exit(-1);
	}

	m_boidType = std::make_shared<BoidType>("resources/boid.png", BOID_DETECTION_RADIUS, BOID_DETECTION_ANGLE);
	for (int i = 0; i < 1500; i++)
	{
		sf::Vector2f position(std::rand() % 1000 + 100, std::rand() % 800 + 100);
		sf::Vector2f velocity(std::rand() % 301 - 150, std::rand() % 301 - 150);
		std::shared_ptr<Boid> boid(std::make_shared<Boid>(m_boidType, position, velocity, BOID_DETECTION_RADIUS));
		m_boids.push_back(boid);
		m_partitioner->insert(boid);
	}

	BoidType::setSeparationWeight(INITIAL_SEPARATION);
	BoidType::setCohesionWeight(INITIAL_COHESION);
	BoidType::setAllignmentWeight(INITIAL_ALLIGNMENT);

	//m_diskGraph.init(m_boids, 30, m_window.getSize());

	m_separationText.setString("Separation");
	m_cohesionText.setString("Cohesion");
	m_allignmentText.setString("Allignmnet");

	m_separationText.setCharacterSize(20);
	m_cohesionText.setCharacterSize(20);
	m_allignmentText.setCharacterSize(20);

	m_separationText.setPosition({ 20, 30 });
	m_cohesionText.setPosition({ 240, 30 });
	m_allignmentText.setPosition({ 460, 30 });
}

void Boids::run()
{
	bool separation(false);
	bool cohesion(false);
	bool allignment(false);

	while (m_window.isOpen())
	{
		float dt = m_clock.restart().asSeconds();
		while (std::optional event = m_window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				m_window.close();
			if (event->is<sf::Event::Resized>())
			{
				sf::Event::Resized* resizedEvent = event->getIf<sf::Event::Resized>();
				m_dimension.x = resizedEvent->size.x;
				m_dimension.y = resizedEvent->size.y;
				
				m_window.setView(sf::View(sf::FloatRect({ 0, 0 }, { m_dimension.x, m_dimension.y })));
				m_partitioner->resize({ 0, 0 }, m_dimension);
				//m_diskGraph.setGridSize(m_window.getSize());
			}
			if (event->is<sf::Event::MouseButtonPressed>())
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
				if (m_separation.mouseOver(mousePos))
					separation = true;
				else if (m_cohesion.mouseOver(mousePos))
					cohesion = true;
				else if (m_allignment.mouseOver(mousePos))
					allignment = true;
			}
			if (event->is<sf::Event::MouseButtonReleased>())
				separation = cohesion = allignment = false;
			if ((separation || cohesion || allignment) && event->is<sf::Event::MouseMoved>())
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
				if (separation)
				{
					m_separation.update(mousePos);
					BoidType::setSeparationWeight(m_separation.getValue());
				}
				else if (cohesion)
				{
					m_cohesion.update(mousePos);
					BoidType::setCohesionWeight(m_cohesion.getValue());
				}
				else if (allignment)
				{
					m_allignment.update(mousePos);
					BoidType::setAllignmentWeight(m_allignment.getValue());
				}
			}
		}

		m_fpsText.setString(sf::String(std::to_string((int)std::round(1.0f / dt))));

		m_window.clear();
		m_updateBoids(dt);
		m_draw();
		m_window.display();
	}
}

void Boids::m_draw()
{
	for (auto boid : m_boids)
		m_boidType->draw(boid, m_window, DEBUG);

	m_separation.draw(m_window);
	m_cohesion.draw(m_window);
	m_allignment.draw(m_window);

	if (DEBUG)
	{
		m_partitioner->drawDebug(m_window);
		//m_diskGraph.setBoids(m_boids, 30);
		//m_diskGraph.drawDebug(m_window);
	}

	m_drawText();
}

void Boids::m_drawText()
{
	m_window.draw(m_separationText);
	m_window.draw(m_cohesionText);
	m_window.draw(m_allignmentText);
	m_window.draw(m_fpsText);
}

void Boids::m_updateBoids(float deltaTime)
{
	m_updateBoidsVelocities(deltaTime);
	m_updateBoidsPositions(deltaTime);
	m_partitioner->pack();
	//m_resetTree();
}

void Boids::m_updateBoidsVelocities(float deltaTime)
{
	for (auto boid : m_boids)
		m_boidType->updateVelocity(boid, m_partitioner, m_boids, deltaTime);
}

void Boids::m_updateBoidsPositions(float deltaTime)
{
	for (auto boid : m_boids) {
		sf::Vector2f oldPosition = boid->getTranslation();
		m_boidType->updatePosition(boid, deltaTime, m_dimension);
		m_partitioner->updateItem(boid);
	}
}