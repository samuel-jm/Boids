#include "Boids.h"

Boids::Boids() :
	m_dimension(new sf::Vector2f(1200, 1000)),
	m_window(sf::VideoMode({ 1200, 1000 }), "Boids"),
	m_clock(), m_deltaTime(0),
	m_quadtree(5, 5, 0, { {0, 0}, {3440, 1440} }, nullptr),
	m_sepWeight(new float(1.f)), m_cohWeight(new float(1.f)), m_allWeight(new float(1.f)),
	m_font("fonts/arial.ttf"),
	m_sepText(m_font), m_cohText(m_font), m_allText(m_font),
	m_sep(sf::IntRect({ 20, 20 }, { 200, 10 })), m_coh(sf::IntRect({ 240, 20 }, { 200, 10 })), m_all(sf::IntRect({ 460, 20 }, { 200, 10 }))
{
	for (int i = 0; i < 1500; i++)
	{
		sf::Vector2f position(std::rand() % 1000 + 100, std::rand() % 800 + 100);
		sf::Vector2f velocity(std::rand() % 300 - 150, std::rand() % 300 - 150);
		m_boids.push_back(std::make_shared<Boid>(m_window, position, velocity, *m_dimension, m_sepWeight, m_cohWeight, m_allWeight, 30, i));
	}

	m_sepText.setString("Seperation");
	m_cohText.setString("Cohesion");
	m_allText.setString("Allignmnet");

	m_sepText.setCharacterSize(20);
	m_cohText.setCharacterSize(20);
	m_allText.setCharacterSize(20);

	m_sepText.setPosition({ 20, 30 });
	m_cohText.setPosition({ 240, 30 });
	m_allText.setPosition({ 460, 30 });
}

Boids::~Boids()
{
	delete m_sepWeight;
	delete m_cohWeight;
	delete m_allWeight;

	delete m_dimension;
}

void Boids::run()
{
	bool sep(false);
	bool coh(false);
	bool all(false);

	while (m_window.isOpen())
	{
		while (std::optional event = m_window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				m_window.close();
			if (event->is<sf::Event::Resized>())
			{
				m_dimension->x = event->getIf<sf::Event::Resized>()->size.x;
				m_dimension->y = event->getIf<sf::Event::Resized>()->size.y;
				
				m_window.setView(sf::View(sf::FloatRect({ 0, 0 }, { m_dimension->x, m_dimension->y })));
			}
			if (event->is<sf::Event::MouseButtonPressed>())
			{
				if (m_sep.getArea().contains(sf::Mouse::getPosition(m_window)))
					sep = true;
				else if (m_coh.getArea().contains(sf::Mouse::getPosition(m_window)))
					coh = true;
				else if (m_all.getArea().contains(sf::Mouse::getPosition(m_window)))
					all = true;
			}
			if (event->is<sf::Event::MouseButtonReleased>())
				sep = coh = all = false;
			if ((sep || coh || all) && event->is<sf::Event::MouseMoved>())
			{
				if (sep)
				{
					m_sep.setValue(sf::Mouse::getPosition(m_window).x);
					*m_sepWeight = m_sep.getValue() / 100.f;
				}
				else if (coh)
				{
					m_coh.setValue(sf::Mouse::getPosition(m_window).x);
					*m_cohWeight = m_coh.getValue() / 100.f;
				}
				else if (all)
				{
					m_all.setValue(sf::Mouse::getPosition(m_window).x);
					*m_allWeight = m_all.getValue() / 100.f;
				}
			}
		}

		m_deltaTime = std::min(m_clock.restart().asSeconds(), 1/15.f);

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

	m_sep.draw(m_window);
	m_coh.draw(m_window);
	m_all.draw(m_window);

	if (DEBUG) m_quadtree.drawDebug(m_window);

	m_drawText();
}

void Boids::m_drawText()
{
	m_window.draw(m_sepText);
	m_window.draw(m_cohText);
	m_window.draw(m_allText);
}

void Boids::m_updateVelocity(float deltaTime)
{
	m_updateTree();
	for (auto boid : m_boids)
	{
		boid->updateVelocity(m_quadtree, m_boids, deltaTime);
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