#pragma once

#include <map>
#include <set>
#include <sstream>

#include "SFML/Graphics.hpp"

#include "Boid.h"

class DiskGraph
{
public:
	void init(std::vector<std::shared_ptr<Boid>> boids, int radius, sf::Vector2u windowSize);
	void setBoids(std::vector<std::shared_ptr<Boid>> boids, int radius, bool updateGrid = true);
	void setGridSize(sf::Vector2u windowSize, bool updateGrid = true);
	std::vector<std::shared_ptr<Boid>> search(const sf::Vector2f& boidPosition);
	void drawDebug(sf::RenderWindow& window);
private:
	std::vector<std::shared_ptr<Boid>> m_boids;
	std::map<std::pair<int, int>, std::set<std::shared_ptr<Boid>>> m_integerGrid;
	sf::Vector2u m_gridSize;

	int m_radius;

	void m_populateGrid();
};

