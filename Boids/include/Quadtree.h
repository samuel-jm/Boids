#pragma once

#include "Boid.h"
#include "IPartitioner.h"

#include <memory>
#include <vector>

template<typename T>
class Quadtree : public IPartitioner<T>
{
public:
	Quadtree(int maxItems, int maxLevels, int level, sf::FloatRect bounds) :
		m_maxItems(maxItems), m_maxLevels(maxLevels), m_level(level), m_bounds(bounds)
	{
	}

	void insert(std::shared_ptr<ITranslatable<T>> item) override
	{
		if (m_tl != nullptr) // the four children are either all null or all initialized
		{
			m_getItemBin(item)->insert(item);
			return;
		}

		m_items.emplace_back(item);

		if ((m_items.size() >= m_maxItems) && (m_level < m_maxLevels))
		{
			m_split();
			for (auto it = m_items.begin(); it != m_items.end(); it++)
			{
				m_getItemBin(*it)->insert(*it);
			}
			m_items.clear();
		}
	}

	void remove(std::shared_ptr<ITranslatable<T>> item) override
	{
		/*sf::FloatRect area({ item->getTranslation().x - item->getRadius(), item->getTranslation().y - item->getRadius() },
			{ item->getRadius() * 2.f, item->getRadius() * 2.f });

		int index = m_getItemBinIndex(item->getTranslation());
		if (index == m_thisTree || m_children[index] == nullptr)
		{
			for (int i = 0; i < m_items.size(); i++)
			{
				if (m_items.at(i)->getID() == item->getID())
				{
					m_items.erase(m_items.begin() + i);
					break;
				}
			}
		}
		else
			return m_children[index]->remove(item);*/
	}

	void updateItem(std::shared_ptr<ITranslatable<T>> item) override
	{

	}

	void clear() override
	{
		if (m_tl == nullptr) return;
		m_tl->clear();
		m_tr->clear();
		m_bl->clear();
		m_br->clear();

		m_tl.reset();
		m_tr.reset();
		m_bl.reset();
		m_br.reset();
	}

	std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> item) override
	{
		return m_tl == nullptr ? m_items : m_getItemBin(item)->search(item);
	}

	void setBounds(const sf::FloatRect& bounds) {
		m_bounds = bounds;
	}

	const sf::FloatRect& getBounds()
	{
		return m_bounds;
	}

	void drawDebug(sf::RenderWindow& window) override
	{
		if (m_tl != nullptr)
		{
			m_tl->drawDebug(window);
			m_tr->drawDebug(window);
			m_bl->drawDebug(window);
			m_br->drawDebug(window);
			return;
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

private:
	std::shared_ptr<Quadtree<T>> m_getItemBin(std::shared_ptr<ITranslatable<T>> item)
	{
		double verticalDividingLine = m_bounds.position.x + m_bounds.size.x * 0.5f;
		double horizontalDividingLine = m_bounds.position.y + m_bounds.size.y * 0.5f;

		const T& translation = item->getTranslation();
		bool top = translation.y < horizontalDividingLine;
		bool right = translation.x > verticalDividingLine;

		if (top)
			return right ? m_tr : m_tl;
		return right ? m_br : m_bl;
	}

	void m_split()
	{
		const float childWidth = m_bounds.size.x / 2;
		const float childHeight = m_bounds.size.y / 2;

		m_tl = std::make_shared<Quadtree<T>>(m_maxItems, m_maxLevels, m_level + 1,
			sf::FloatRect({ m_bounds.position.x, m_bounds.position.y }, { childWidth, childHeight }));
		m_tr = std::make_shared<Quadtree<T>>(m_maxItems, m_maxLevels, m_level + 1,
			sf::FloatRect({ m_bounds.position.x + childWidth, m_bounds.position.y }, { childWidth, childHeight }));
		m_bl = std::make_shared<Quadtree<T>>(m_maxItems, m_maxLevels, m_level + 1,
			sf::FloatRect({ m_bounds.position.x, m_bounds.position.y + childHeight }, { childWidth, childHeight }));
		m_br = std::make_shared<Quadtree<T>>(m_maxItems, m_maxLevels, m_level + 1,
			sf::FloatRect({ m_bounds.position.x + childWidth, m_bounds.position.y + childHeight }, { childWidth, childHeight }));
	}

	std::shared_ptr<Quadtree<T>> m_tl;
	std::shared_ptr<Quadtree<T>> m_tr;
	std::shared_ptr<Quadtree<T>> m_bl;
	std::shared_ptr<Quadtree<T>> m_br;

	int m_maxItems;
	int m_maxLevels;

	std::vector<std::shared_ptr<ITranslatable<T>>> m_items;

	int m_level;

	sf::FloatRect m_bounds;
};
