#include "Boids.h"

Boids::Boids() :
	m_dimension(new sf::Vector2f(1200, 1000)),
	m_window(sf::VideoMode({ 1200, 1000 }), "Boids"),
	m_clock(), m_deltaTime(0),
	m_quadtree(15, 5, 0, { {0, 0}, {(float)m_window.getSize().x, (float)m_window.getSize().y} }, nullptr),
	m_font("fonts/arial.ttf"),
	m_separationText(m_font), m_cohesionText(m_font), m_allignmentText(m_font),
	m_fpsText(m_font),
	m_separation(sf::IntRect({ 20, 20 }, { 200, 10 })), m_cohesion(sf::IntRect({ 240, 20 }, { 200, 10 })), m_allignment(sf::IntRect({ 460, 20 }, { 200, 10 }))
{
	for (int i = 0; i < 150; i++)
	{
		sf::Vector2f position(std::rand() % 1000 + 100, std::rand() % 800 + 100);
		sf::Vector2f velocity(std::rand() % 300 - 150, std::rand() % 300 - 150);
		m_boids.push_back(std::make_shared<Boid>(m_window, position, velocity, *m_dimension, 30));
	}
	Boid::setSeparationWeight(1.0f);
	Boid::setCohesionWeight(1.0f);
	Boid::setAllignmentWeight(1.0f);

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
				m_diskGraph.setGridSize(m_window.getSize());
			}
			if (event->is<sf::Event::MouseButtonPressed>())
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
				if (m_separation.getArea().contains(mousePos))
					separation = true;
				else if (m_cohesion.getArea().contains(mousePos))
					cohesion = true;
				else if (m_allignment.getArea().contains(mousePos))
					allignment = true;
			}
			if (event->is<sf::Event::MouseButtonReleased>())
				separation = cohesion = allignment = false;
			if ((separation || cohesion || allignment) && event->is<sf::Event::MouseMoved>())
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
				if (separation)
				{
					m_separation.setValue(mousePos.x);
					Boid::setSeparationWeight(m_separation.getValue() / 100.f);
				}
				else if (cohesion)
				{
					m_cohesion.setValue(mousePos.x);
					Boid::setCohesionWeight(m_cohesion.getValue() / 100.f);
				}
				else if (allignment)
				{
					m_allignment.setValue(mousePos.x);
					Boid::setAllignmentWeight(m_allignment.getValue() / 100.f);
				}
			}
		}

		m_fpsText.setString(sf::String(std::to_string((int)std::round(1.0f / m_deltaTime))));

		m_window.clear();
		m_updateVelocity(m_deltaTime);
		m_draw();
		m_window.display();
	}
}

void Boids::m_draw()
{
	for (auto boid : m_boids)
	{
		boid->draw(DEBUG, m_deltaTime);
	}

	m_separation.draw(m_window);
	m_cohesion.draw(m_window);
	m_allignment.draw(m_window);

	if (DEBUG)
	{
		//m_quadtree.drawDebug(m_window);
		m_diskGraph.setBoids(m_boids, 30);
		m_diskGraph.drawDebug(m_window);
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

void Boids::m_updateVelocity(float deltaTime)
{
	m_updateTree();
	for (auto boid : m_boids)
	{
		boid->updateVelocity(m_diskGraph, m_boids, deltaTime);
	}
}

void Boids::m_updateTree()
{
	m_quadtree.clear();
	for (auto boid : m_boids)
	{
		m_quadtree.insert(boid);
	}

}