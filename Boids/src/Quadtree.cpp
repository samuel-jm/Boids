#include "Quadtree.h"

Quadtree::Quadtree(int maxBoids, int maxLevels, int level, sf::FloatRect bounds, Quadtree* parent) :
	m_maxObjects(maxBoids), m_maxLevels(maxLevels), m_level(level), m_bounds(bounds), m_parent(parent)
{
}

void Quadtree::insert(std::shared_ptr<Boid> boid)
{
	if (!m_bounds.contains(boid->getPoint()))
	{
		return;
	}

	if (m_children[0] != nullptr)
	{
		sf::FloatRect area({ boid->getPoint().x - boid->getRadius(), boid->getPoint().y - boid->getRadius() },
			{ boid->getRadius() * 2.f, boid->getRadius() * 2.f });

		int indexToPlaceObject =
			m_getChildIndexForObjects(area);
		if (indexToPlaceObject != m_thisTree)
		{
			m_children[indexToPlaceObject]->insert(boid);
			return;
		}
	}

	m_boids.emplace_back(boid);

	if (m_boids.size() > m_maxObjects && m_level < m_maxLevels && m_children[0] == nullptr)
	{
		m_split();
		auto it = m_boids.begin();
		while (it != m_boids.end())
		{
			sf::FloatRect area({ boid->getPoint().x - boid->getRadius(), boid->getPoint().y - boid->getRadius() },
				{ boid->getRadius() * 2.f, boid->getRadius() * 2.f });

			auto obj = *it;
			int indexToPlaceObject = m_getChildIndexForObjects(area);
			if (indexToPlaceObject != m_thisTree)
			{
				m_children[indexToPlaceObject]->insert(obj);
				it = m_boids.erase(it);
			}
			else
				it++;
		}
	}
}

void Quadtree::remove(std::shared_ptr<Boid> boid)
{
	sf::FloatRect area({ boid->getPoint().x - boid->getRadius(), boid->getPoint().y - boid->getRadius() },
		{ boid->getRadius() * 2.f, boid->getRadius() * 2.f });

	int index = m_getChildIndexForObjects(area);
	if (index == m_thisTree || m_children[index] == nullptr)
	{
		for (int i = 0; i < m_boids.size(); i++)
		{
			if (m_boids.at(i)->getID() == boid->getID())
			{
				m_boids.erase(m_boids.begin() + i);
				break;
			}
		}
	}
	else
		return m_children[index]->remove(boid);
}

void Quadtree::clear()
{
	m_boids.clear();
	for (int i = 0; i < 4; i++)
	{
		if (m_children[i] != nullptr)
		{
			m_children[i]->clear();
			m_children[i] = nullptr;
		}
	}
}

std::vector<std::shared_ptr<Boid>> Quadtree::search(const sf::FloatRect& area)
{
	std::vector<std::shared_ptr<Boid>> possibleOverlaps;
	m_search(area, possibleOverlaps);

	std::vector<std::shared_ptr<Boid>> returnList;

	for (auto possibleBoid : possibleOverlaps)
	{
		if (area.contains(possibleBoid->getPoint()))
	  {
			returnList.emplace_back(possibleBoid);
		}
	}
	return returnList;
}

const sf::FloatRect& Quadtree::getBounds() const
{
	return m_bounds;
}

void Quadtree::drawDebug(sf::RenderWindow& window)
{
	if (m_children[0] != nullptr)
	{
		for (int i = 0; i < 4; i++)
		{
			m_children[i]->drawDebug(window);
		}
	}

	sf::Vector2f size(m_bounds.size.x, m_bounds.size.y);
	sf::Vector2f pos(m_bounds.position.x, m_bounds.position.y);
	sf::RectangleShape shape(size);

	shape.setPosition(pos);
	shape.setOutlineColor(sf::Color::Red);
	shape.setOutlineThickness(0.5f);
	shape.setFillColor(sf::Color::Transparent);

	window.draw(shape);
}

void Quadtree::m_search(const sf::FloatRect& area, std::vector<std::shared_ptr<Boid>>& overlappingObjects)
{
	overlappingObjects.insert(overlappingObjects.end(), m_boids.begin(), m_boids.end());
	if (m_children[0] != nullptr)
	{
		int index = m_getChildIndexForObjects(area);
		if (index == m_thisTree)
		{
			for (int i = 0; i < 4; i++)
			{
				if (m_children[i]->getBounds().findIntersection(area) != std::nullopt)
				{
					m_children[i]->m_search(area, overlappingObjects);
				}
			}
		}
		else
		{
			m_children[index]->m_search(area, overlappingObjects);
		}
	}
}

int Quadtree::m_getChildIndexForObjects(const sf::FloatRect& boidPosition)
{
	int index = -1;
	double verticalDividingLine = m_bounds.position.x + m_bounds.size.x * 0.5f;
	double horizontalDividingLine = m_bounds.position.y + m_bounds.size.y * 0.5f;

	bool north = boidPosition.position.y < horizontalDividingLine &&
		(boidPosition.size.y + boidPosition.position.y < horizontalDividingLine);
	bool south = boidPosition.position.y > horizontalDividingLine;
	bool west = boidPosition.position.x < verticalDividingLine &&
		(boidPosition.position.x + boidPosition.size.x < verticalDividingLine);
	bool east = boidPosition.position.x > verticalDividingLine;

	if (east)
	{
		if (north)
			index = m_childNE;
		else if (south)
			index = m_childSE;
	}
	else if (west)
	{
		if (north)
			index = m_childNW;
		else if (south)
			index = m_childSW;
	}

	return index;
}

void Quadtree::m_split()
{
	const float childWidth = m_bounds.size.x / 2;
	const float childHeight = m_bounds.size.y / 2;

	m_children[m_childNE] = std::make_shared<Quadtree>(m_maxObjects, m_maxLevels, m_level + 1,
		sf::FloatRect({ m_bounds.position.x + childWidth, m_bounds.position.y }, { childWidth, childHeight }), this);
	m_children[m_childNW] = std::make_shared<Quadtree>(m_maxObjects, m_maxLevels, m_level + 1,
		sf::FloatRect({ m_bounds.position.x, m_bounds.position.y }, { childWidth, childHeight }), this);
	m_children[m_childSW] = std::make_shared<Quadtree>(m_maxObjects, m_maxLevels, m_level + 1,
		sf::FloatRect({ m_bounds.position.x, m_bounds.position.y + childHeight }, { childWidth, childHeight }), this);
	m_children[m_childSE] = std::make_shared<Quadtree>(m_maxObjects, m_maxLevels, m_level + 1,
		sf::FloatRect({ m_bounds.position.x + childWidth, m_bounds.position.y + childHeight }, { childWidth, childHeight }), this);
}