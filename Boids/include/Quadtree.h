#pragma once

#include "Boid.h"
#include "IPartitioner.h"

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>

template<typename T>
class Quadtree : public IPartitioner<T>
{
public:
	Quadtree(int maxItems, int maxLevels, T position, T size) :
		m_root(new Node(0, position, size, *this, nullptr)),
		m_cache(std::unordered_map<std::shared_ptr<ITranslatable<T>>, Node*>()),
		m_maxItems(maxItems),
		m_maxLevels(maxLevels)
	{
	}

	void insert(std::shared_ptr<ITranslatable<T>> item) override { m_root->getNodeContainingItem(item)->insert(item); }
	void remove(std::shared_ptr<ITranslatable<T>> item) override { m_root->getNodeContainingItem(item)->remove(item); }
	void updateItem(std::shared_ptr<ITranslatable<T>> item) override 
	{
		Node* oldNode = m_cache[item];
		Node* newNode = m_root->getNodeContainingItem(item);

		if (oldNode != newNode)
		{
			oldNode->remove(item);
			newNode->insert(item);
		}
	}

	void clear() override { m_root->clear(); }
	void resize(const T& position, const T& size)
	{
		clear();
		m_root->position = position;
		m_root->size = size;
		for (auto it = m_cache.begin(); it != m_cache.end(); it++)
		{
			m_root->insert(it->first);
		}
	}
	std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> item) override { return m_root->getNodeContainingItem(item)->items; }
	void drawDebug(sf::RenderWindow& window) override { m_root->drawDebug(window); }

	void pack()
	{
		m_root->merge();
	}

private:
	struct Node;

	Node* m_root;
	std::unordered_map<std::shared_ptr<ITranslatable<T>>, Node*> m_cache;

	int m_maxItems;
	int m_maxLevels;

	struct Node
	{
		Quadtree<T>& quadtree;
		Node* parent;
		std::vector<std::shared_ptr<ITranslatable<T>>> items;
		T position;
		T size;
		std::unique_ptr<Node> tl;
		std::unique_ptr<Node> tr;
		std::unique_ptr<Node> bl;
		std::unique_ptr<Node> br;

		int level;

		Node(int level, T position, T size, Quadtree<T>& quadtree, Node* parent) :
			level(level), position(position), size(size), quadtree(quadtree), parent(parent)
		{
		}

		void insert(std::shared_ptr<ITranslatable<T>> item)
		{
			items.push_back(item);
			quadtree.m_cache[item] = this;

			if ((items.size() > quadtree.m_maxItems) && (level < quadtree.m_maxLevels))
			{
				split();
			}
		}

		void remove(std::shared_ptr<ITranslatable<T>> item)
		{
			items.erase(std::find(items.begin(), items.end(), item));
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

		void drawDebug(sf::RenderWindow& window)
		{
			static bool numItems(false);
			static bool outline(true);

			if (tl != nullptr)
			{
				tl->drawDebug(window);
				tr->drawDebug(window);
				bl->drawDebug(window);
				br->drawDebug(window);
				return;
			}

			if (outline)
			{
				sf::RectangleShape shape(size);

				shape.setPosition(position);
				shape.setOutlineColor(sf::Color::Red);
				shape.setOutlineThickness(0.5f);
				shape.setFillColor(sf::Color::Transparent);

				window.draw(shape);
			}

			if (numItems)
			{
				sf::Vector2f pos(position.x, position.y);
				sf::Font f("fonts/arial.ttf");
				sf::Text t(f);
				std::stringstream ss;
				ss << items.size() << ", " << "(" << position.x << ", " << position.y << ")";
				t.setString(ss.str());
				t.setPosition(pos);
				t.setCharacterSize(18);
				window.draw(t);
			}
		}

		Node* getNodeContainingItem(std::shared_ptr<ITranslatable<T>> item)
		{
			if (tl == nullptr) return this;

			double verticalDividingLine = position.x + size.x * 0.5f;
			double horizontalDividingLine = position.y + size.y * 0.5f;

			const T& translation = item->getTranslation();
			bool top = translation.y < horizontalDividingLine;
			bool right = translation.x > verticalDividingLine;

			Node* nextNode = [&]()
				{
					if (top)
						return right ? tr.get() : tl.get();
					return right ? br.get() : bl.get();
				}();

			return nextNode->getNodeContainingItem(item);
		}

		void split()
		{
			// Only leaf-nodes can be split
			if (tl != nullptr) return;

			T childSize(size.x / 2, size.y / 2);

			tl = std::make_unique<Node>(level + 1, position, childSize, quadtree, this);
			tr = std::make_unique<Node>(level + 1, T(position.x + childSize.x, position.y), T(childSize.x, childSize.y), quadtree, this);
			bl = std::make_unique<Node>(level + 1, T(position.x, position.y + childSize.y), T(childSize.x, childSize.y), quadtree, this);
			br = std::make_unique<Node>(level + 1, T(position.x + childSize.x, position.y + childSize.y), T(childSize.x, childSize.y), quadtree, this);

			// Items in this node are distributed to children nodes
			for (auto it = items.begin(); it != items.end(); it++)
			{
				getNodeContainingItem(*it)->insert(*it);
			}
			items.clear();
		}

		int merge()
		{
			if (tl == nullptr) return items.size();

			int numContainedItems = tl->merge() + tr->merge() + bl->merge() + br->merge();
			if (numContainedItems <= quadtree.m_maxItems)
			{
				items.insert(items.end(), std::make_move_iterator(tl->items.begin()), std::make_move_iterator(tl->items.end()));
				items.insert(items.end(), std::make_move_iterator(tr->items.begin()), std::make_move_iterator(tr->items.end()));
				items.insert(items.end(), std::make_move_iterator(bl->items.begin()), std::make_move_iterator(bl->items.end()));
				items.insert(items.end(), std::make_move_iterator(br->items.begin()), std::make_move_iterator(br->items.end()));

				assert(numContainedItems == items.size());

				for (auto it = items.begin(); it != items.end(); it++)
				{
					quadtree.m_cache[*it] = this;
				}

				tl.reset();
				tr.reset();
				bl.reset();
				br.reset();
			}

			return numContainedItems;
		}
	};
};
