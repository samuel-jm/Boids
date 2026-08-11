#pragma once

#include "Boid.h"

#include <memory>
#include <vector>

class Quadtree
{
public:
	Quadtree(int maxBoids, int maxLevels, int level, sf::FloatRect bounds, Quadtree* parent);

	void insert(std::shared_ptr<Boid> boid);
	void remove(std::shared_ptr<Boid> boid);
	void clear();

	std::vector<std::shared_ptr<Boid>> search(const sf::Vector2f& boidPosition);

	const sf::FloatRect& getBounds() const;

	void drawDebug(sf::RenderWindow& window);

private:
	std::vector<std::shared_ptr<Boid>> m_search(const sf::Vector2f& boidPosition);
	int m_getChildIndexForObjects(const sf::Vector2f& boidPosition);
	void m_split();

	static const int m_thisTree = -1;
	static const int m_childNE = 0;
	static const int m_childNW = 1;
	static const int m_childSW = 2;
	static const int m_childSE = 3;

	int m_maxObjects;
	int m_maxLevels;

	Quadtree* m_parent;
	std::shared_ptr<Quadtree> m_children[4];

	std::vector<std::shared_ptr<Boid>> m_boids;

	int m_level;

	sf::FloatRect m_bounds;
};
