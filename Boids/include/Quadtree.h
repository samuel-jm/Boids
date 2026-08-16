#pragma once

#include "Boid.h"
#include "IPartitioner.h"

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

template<typename T>
class Quadtree : public IPartitioner<T>
{
public:
	Quadtree(int maxItems, int maxLevels, sf::FloatRect bounds) :
		m_root(0, bounds, *this),
		m_cache(std::unordered_map<std::shared_ptr<ITranslatable<T>>, std::shared_ptr<Node>>()),
		m_maxItems(maxItems),
		m_maxLevels(maxLevels)
	{
	}


	void insert(std::shared_ptr<ITranslatable<T>> item) override { m_root.insert(item); }
	void remove(std::shared_ptr<ITranslatable<T>> item) override { m_root.remove(item); }
	void updateItem(std::shared_ptr<ITranslatable<T>> item) override { m_root.updateItem(item); }
	void clear() override { m_root.clear(); }
	std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> itemPosition) override { return m_root.search(itemPosition); }
	void drawDebug(sf::RenderWindow& window) override { m_root.drawDebug(window); }

private:
	struct Node;

	Node m_root;
	std::unordered_map<std::shared_ptr<ITranslatable<T>>, std::shared_ptr<Node>> m_cache;

	int m_maxItems;
	int m_maxLevels;

	struct Node
	{
		const Quadtree<T>& quadtree;
		std::vector<std::shared_ptr<ITranslatable<T>>> items;
		sf::FloatRect bounds;
		std::shared_ptr<Node> tl;
		std::shared_ptr<Node> tr;
		std::shared_ptr<Node> bl;
		std::shared_ptr<Node> br;

		int level;

		Node(int level, sf::FloatRect bounds, const Quadtree<T>& quadtree) :
			level(level), bounds(bounds), quadtree(quadtree)
		{
		}

		void insert(std::shared_ptr<ITranslatable<T>> item)
		{
			if (tl != nullptr) // the four children are either all null or all initialized
			{
				getItemBin(item)->insert(item);
				return;
			}

			items.emplace_back(item);
			//m_cache[item] = this;

			if ((items.size() >= quadtree.m_maxItems) && (level < quadtree.m_maxLevels))
			{
				split();
				for (auto it = items.begin(); it != items.end(); it++)
				{
					getItemBin(*it)->insert(*it);
				}
				items.clear();
			}
		}

		void remove(std::shared_ptr<ITranslatable<T>> item)
		{
			/*sf::FloatRect area({ item->getTranslation().x - item->getRadius(), item->getTranslation().y - item->getRadius() },
				{ item->getRadius() * 2.f, item->getRadius() * 2.f });

			int index = getItemBinIndex(item->getTranslation());
			if (index == thisTree || children[index] == nullptr)
			{
				for (int i = 0; i < items.size(); i++)
				{
					if (items.at(i)->getID() == item->getID())
					{
						items.erase(items.begin() + i);
						break;
					}
				}
			}
			else
				return children[index]->remove(item);*/
		}

		void updateItem(std::shared_ptr<ITranslatable<T>> item)
		{

		}

		void clear()
		{
			if (tl == nullptr) return;
			tl->clear();
			tr->clear();
			bl->clear();
			br->clear();

			tl.reset();
			tr.reset();
			bl.reset();
			br.reset();
		}

		std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> item)
		{
			return tl == nullptr ? items : getItemBin(item)->search(item);
		}

		std::shared_ptr<Node> getItemBin(std::shared_ptr<ITranslatable<T>> item)
		{
			double verticalDividingLine = bounds.position.x + bounds.size.x * 0.5f;
			double horizontalDividingLine = bounds.position.y + bounds.size.y * 0.5f;

			const T& translation = item->getTranslation();
			bool top = translation.y < horizontalDividingLine;
			bool right = translation.x > verticalDividingLine;

			if (top)
				return right ? tr : tl;
			return right ? br : bl;
		}

		void split()
		{
			const float childWidth = bounds.size.x / 2;
			const float childHeight = bounds.size.y / 2;

			tl = std::make_shared<Node>(level + 1, sf::FloatRect({ bounds.position.x, bounds.position.y }, { childWidth, childHeight }), quadtree);
			tr = std::make_shared<Node>(level + 1, sf::FloatRect({ bounds.position.x + childWidth, bounds.position.y }, { childWidth, childHeight }), quadtree);
			bl = std::make_shared<Node>(level + 1, sf::FloatRect({ bounds.position.x, bounds.position.y + childHeight }, { childWidth, childHeight }), quadtree);
			br = std::make_shared<Node>(level + 1, sf::FloatRect({ bounds.position.x + childWidth, bounds.position.y + childHeight }, { childWidth, childHeight }), quadtree);
		}

		void drawDebug(sf::RenderWindow& window)
		{
			if (tl != nullptr)
			{
				tl->drawDebug(window);
				tr->drawDebug(window);
				bl->drawDebug(window);
				br->drawDebug(window);
				return;
			}

			sf::Vector2f size(bounds.size.x, bounds.size.y);
			sf::Vector2f pos(bounds.position.x, bounds.position.y);
			sf::RectangleShape shape(size);

			shape.setPosition(pos);
			shape.setOutlineColor(sf::Color::Red);
			shape.setOutlineThickness(0.5f);
			shape.setFillColor(sf::Color::Transparent);

			window.draw(shape);
		}
	};
};
