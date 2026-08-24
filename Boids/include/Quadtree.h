#pragma once

#include "IPartitioner.h"

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>

/// <summary>
/// This class represents a Quadtree and is used to efficiently store and
/// retrieve objects which implement the ITranslatable interface, that is,
/// objects which have a 2D position.
/// The Quadtree itself implements the IPartitioner interface which defines
/// the methods the Quadtree must implement in order to function, this allows
/// users of IPartitioner flexibility in deciding exactly how their ITranslatable
/// items get partitioned.
/// </summary>
/// <typeparam name="T">The type which defines a 2D position, such as sf::Vector2f. Must contain .x and .y public members</typeparam>
template<typename T>
class Quadtree : public IPartitioner<T>
{
public:
	/// <summary>
	/// This constructor sets the maximum number of items each node in the Quadtree
	/// can contain, the maximum height of the Quadtree, and the position and
	/// size of the Quadtree
	/// </summary>
	/// <param name="maxItems">The maximum number of items each node can contain</param>
	/// <param name="maxLevels">The maximum height of the Quadtree</param>
	/// <param name="position">The top-left position of the tree</param>
	/// <param name="size">The width and height of the tree</param>
	Quadtree(int maxItems, int maxLevels, const T& position, const T& size) :
		m_root(new Node(0, position, size, *this, nullptr)),
		m_cache(std::unordered_map<std::shared_ptr<ITranslatable<T>>, Node*>()),
		m_maxItems(maxItems),
		m_maxLevels(maxLevels)
	{
	}

	/// <summary>
	/// Insert the given item into the node where it belongs in the Quadtree
	/// </summary>
	/// <param name="item">The item to be inserted</param>
	void insert(std::shared_ptr<ITranslatable<T>> item) override { m_root->getNodeForItem(item)->insert(item); }
	
	/// <summary>
	/// Remove the given item from the Quadtree
	/// </summary>
	/// <param name="item">The item to be removed</param>
	void remove(std::shared_ptr<ITranslatable<T>> item) override { m_root->getNodeForItem(item)->remove(item); }
	
	/// <summary>
	/// Update the item's place in the Quadtree
	/// </summary>
	/// <param name="item">The item whose place is to be updated</param>
	void updateItem(std::shared_ptr<ITranslatable<T>> item) override 
	{
		Node* oldNode = m_cache[item];
		Node* newNode = m_root->getNodeForItem(item);

		if (oldNode != newNode)
		{
			oldNode->remove(item);
			newNode->insert(item);
		}
	}

	/// <summary>
	/// Reset the Quadtree to just an empty root node
	/// </summary>
	void clear() override { m_root->clear(); }

	/// <summary>
	/// Resize the Quadtree to the given position and size
	/// </summary>
	/// <param name="position">The new top-left position for the Quadtree</param>
	/// <param name="size">The new size for the Quadtree</param>
	void resize(const T& position, const T& size) override
	{
		clear();
		m_root->position = position;
		m_root->size = size;
		for (auto it = m_cache.begin(); it != m_cache.end(); it++)
		{
			m_root->insert(it->first);
		}
	}

	/// <summary>
	/// Search the Quadtree and return all the items in the node containing the input item, including the item itself
	/// </summary>
	/// <param name="item">The item whose containing node is to be found</param>
	/// <returns>The items within the node containing the input item</returns>
	std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> item) override { return m_root->getNodeForItem(item)->items; }
	
	/// <summary>
	/// Renders each of the nodes to the given window
	/// </summary>
	/// <param name="window">The window to render to</param>
	void drawDebug(sf::RenderWindow& window) override { m_root->drawDebug(window); }

	/// <summary>
	/// If there are any leaf nodes which can be merged into their parent node, this method will do that
	/// </summary>
	void pack() override
	{
		m_root->merge();
	}

private:
	struct Node;

	Node* m_root;

	/// <summary>
	/// This member is used to keep track of which items are contained within which leaf-nodes
	/// </summary>
	std::unordered_map<std::shared_ptr<ITranslatable<T>>, Node*> m_cache;

	int m_maxItems;
	int m_maxLevels;

	/// <summary>
	/// This struct represents a single node in the Quadtree
	/// </summary>
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

		/// <summary>
		/// The level of the node in the Quadtree, the root node has level = 0
		/// </summary>
		int level;

		/// <summary>
		/// This constructor is used to initialise a new node
		/// </summary>
		/// <param name="level">The level of the node, the root node has level 0, every other node has a higher level</param>
		/// <param name="position">The top-left position of the node</param>
		/// <param name="size">The width and height of the node</param>
		/// <param name="quadtree">The Quadtree instance containing the node</param>
		/// <param name="parent">The parent node of this node</param>
		Node(int level, T position, T size, Quadtree<T>& quadtree, Node* parent) :
			level(level), position(position), size(size), quadtree(quadtree), parent(parent)
		{
		}

		/// <summary>
		/// Inserts the given item into this node and then splits the node into four
		/// child nodes if the number of items in this node exceeds the maximum number
		/// of items a node can have.
		/// </summary>
		/// <param name="item">The item to insert</param>
		void insert(std::shared_ptr<ITranslatable<T>> item)
		{
			items.push_back(item);
			quadtree.m_cache[item] = this;

			if ((items.size() > quadtree.m_maxItems) && (level < quadtree.m_maxLevels))
			{
				split();
			}
		}

		/// <summary>
		/// Removes the given item from this node, if it is present
		/// </summary>
		/// <param name="item">The item to remove</param>
		void remove(std::shared_ptr<ITranslatable<T>> item)
		{
			auto it = std::find(items.begin(), items.end(), item);
			if(it != items.end())
				items.erase(it);
		}

		/// <summary>
		/// Recursively deletes all the child nodes and clears the list of items.
		/// </summary>
		void clear()
		{
			if (tl == nullptr)
			{
				items.clear();
				return;
			}
			tl->clear();
			tr->clear();
			bl->clear();
			br->clear();

			tl.reset();
			tr.reset();
			bl.reset();
			br.reset();

			items.clear();
		}

		/// <summary>
		/// Draws a representation of the Quadtree with a couple of options for
		/// what to draw, the outline section draws the outline of the node while
		/// the numItems section draws text showing the numbers of items within each node
		/// </summary>
		/// <param name="window"></param>
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

		/// <summary>
		/// Recursively traverses from node to child node and returns the node 
		/// in which the given item belongs (Whether or not it is there at the moment)
		/// </summary>
		/// <param name="item">The item whose place is to be found</param>
		/// <returns>The node the given item belongs in</returns>
		Node* getNodeForItem(std::shared_ptr<ITranslatable<T>> item)
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

			return nextNode->getNodeForItem(item);
		}

		/// <summary>
		/// Splits this node into four children nodes, and distributes its items between them
		/// according to where they belong.
		/// </summary>
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
				getNodeForItem(*it)->insert(*it);
			}
			items.clear();
		}

		/// <summary>
		/// Recursively combines child nodes into their parent node as long as the merge wouldn't result
		/// in a node having more items than its capacity allows.
		/// </summary>
		/// <returns>The number of items this node now has, should not exceed m_maxItems</returns>
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
