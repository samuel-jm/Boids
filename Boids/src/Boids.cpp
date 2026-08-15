#include "Boids.h"

const float INITIAL_SEPARATION = 1.f;
const float INITIAL_COHESION = 0.3f;
const float INITIAL_ALLIGNMENT = 1.f;

Boids::Boids() :
	m_dimension(new sf::Vector2f(1200, 1000)),
	m_window(sf::VideoMode({ 1200, 1000 }), "Boids"),
	m_clock(), m_deltaTime(0),
	m_quadtree(15, 4, 0, { {0, 0}, {(float)m_window.getSize().x, (float)m_window.getSize().y} }),
	m_font("fonts/arial.ttf"),
	m_separationText(m_font), m_cohesionText(m_font), m_allignmentText(m_font),
	m_fpsText(m_font),
	m_separation({ 20, 20 }, { 200, 10 }, 0.f, 1.f, INITIAL_SEPARATION),
	m_cohesion({ 240, 20 }, { 200, 10 }, 0.f, 0.5f, INITIAL_COHESION),
	m_allignment({ 460, 20 }, { 200, 10 }, 0.f, 2.f, INITIAL_ALLIGNMENT)
{
	for (int i = 0; i < 1500; i++)
	{
		sf::Vector2f position(std::rand() % 1000 + 100, std::rand() % 800 + 100);
		sf::Vector2f velocity(std::rand() % 301 - 150, std::rand() % 301 - 150);
		m_boids.push_back(std::make_shared<Boid>(m_window, position, velocity, *m_dimension, 30));
	}
	Boid::setSeparationWeight(INITIAL_SEPARATION);
	Boid::setCohesionWeight(INITIAL_COHESION);
	Boid::setAllignmentWeight(INITIAL_ALLIGNMENT);

	m_diskGraph.init(m_boids, 30, m_window.getSize());

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

Boids::~Boids()
{
	delete m_dimension;
}

void Boids::run()
{
	bool separation(false);
	bool cohesion(false);
	bool allignment(false);

	while (m_window.isOpen())
	{
		m_deltaTime = m_clock.restart().asSeconds();
		while (std::optional event = m_window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				m_window.close();
			if (event->is<sf::Event::Resized>())
			{
				sf::Event::Resized* resizedEvent = event->getIf<sf::Event::Resized>();
				m_dimension->x = resizedEvent->size.x;
				m_dimension->y = resizedEvent->size.y;
				
				m_window.setView(sf::View(sf::FloatRect({ 0, 0 }, { m_dimension->x, m_dimension->y })));
				m_quadtree.setBounds({ { 0, 0 }, sf::Vector2f(m_window.getSize()) });
				m_diskGraph.setGridSize(m_window.getSize());
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
					Boid::setSeparationWeight(m_separation.getValue());
				}
				else if (cohesion)
				{
					m_cohesion.update(mousePos);
					Boid::setCohesionWeight(m_cohesion.getValue());
				}
				else if (allignment)
				{
					m_allignment.update(mousePos);
					Boid::setAllignmentWeight(m_allignment.getValue());
				}
			}
		}

		m_fpsText.setString(sf::String(std::to_string((int)std::round(1.0f / m_deltaTime))));

		m_window.clear();
		m_updateBoids(m_deltaTime);
		m_draw();
		m_window.display();
	}
}

void Boids::m_draw()
{
	for (auto boid : m_boids)
	{
		boid->draw(m_window, DEBUG);
	}

	m_separation.draw(m_window);
	m_cohesion.draw(m_window);
	m_allignment.draw(m_window);

	if (DEBUG)
	{
		m_quadtree.drawDebug(m_window);
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
	m_updateTree();
}

void Boids::m_updateBoidsVelocities(float deltaTime)
{
	for (auto boid : m_boids)
		boid->updateVelocity(m_quadtree, m_boids, deltaTime);
}

void Boids::m_updateBoidsPositions(float deltaTime)
{
	for (auto boid : m_boids) {
		sf::Vector2f oldPosition = boid->getTranslation();
		boid->updatePosition(deltaTime);
		m_quadtree.updateItem(boid);
	}
}

void Boids::m_updateTree()
{
	m_quadtree.clear();
	for (auto boid : m_boids)
	{
		m_quadtree.insert(boid);
	}
	m_diskGraph.setBoids(m_boids, 30);
}