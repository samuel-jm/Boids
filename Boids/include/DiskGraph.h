#pragma once

#include <map>
#include <set>
#include <sstream>

#include "SFML/Graphics.hpp"

#include "IPartitioner.h"

/// <summary>
/// This class implements IPartitioner and works as follows: The boids  have a certain detection radius which is given
/// as an argument to the constructor. This radius determines the size of the grid cells that cover the window which is
/// twice the given radius. The floor of a boid's position divided by twice the radius determines which cell it belongs in.
/// This guarantees that if a boid has visible neighbours then they are either in that boid's cell or one of its eight neighbouring
/// cells.
/// This IPartitioner was inspired by https://en.wikipedia.org/wiki/Unit_disk_graph
/// </summary>
/// <typeparam name="T">The type which defines a 2D position, such as sf::Vector2f. Must contain .x and .y public members</typeparam>
template <typename T>
class DiskGraph : public IPartitioner<T>
{
public:
	//void init(std::vector<std::shared_ptr<Boid>> boids, int radius, sf::Vector2u windowSize);
	//void setBoids(std::vector<std::shared_ptr<Boid>> boids, int radius, bool updateGrid = true);
	//void setGridSize(sf::Vector2u windowSize, bool updateGrid = true);
	//std::vector<std::shared_ptr<Boid>> search(const sf::Vector2f& boidPosition);
	//void drawDebug(sf::RenderWindow& window);

	/// <summary>
	/// This constructor initialises the member variables and
	/// sets the flag for every cell that says it needs to 
	/// recalculate its list of neighbours in the next pass
	/// </summary>
	/// <param name="radius">The radius of the items to be partitioned, half the width/height of the grid cells</param>
	/// <param name="position">The top-left position of the DiskGraph</param>
	/// <param name="size">The widht and height of the DiskGraph</param>
	DiskGraph(int radius, const T& position, const T& size) :
		m_radius(radius),
		m_position(position),
		m_windowSize(size),
		m_gridSize(int(size.x) / (2 * radius), int(size.y) / (2 * radius))
	{
		for (int y = 0; y < m_gridSize.y; y++)
		{
			for (int x = 0; x < m_gridSize.x; x++)
			{
				m_shouldRecalculateItemNeighboursCache[{x, y}] = true;
			}
		}
	}

	/// <summary>
	/// Inserts the given item into the DiskGraph and updates the 
	/// various maps for efficient item retrieval.
	/// </summary>
	/// <param name="item">The item to be inserted</param>
	void insert(std::shared_ptr<ITranslatable<T>> item) override
	{
		m_items.push_back(item);

		T position = item->getTranslation();
		std::pair<int, int> itemGridCell = m_getItemGridCell(item);
		m_integerGrid[itemGridCell].insert(item);
		m_itemCellCache[item] = itemGridCell;

		m_setUpdateCellAndNeighbours(itemGridCell);
	}

	/// <summary>
	/// Removes the given item from the DiskGraph if it exists
	/// and updates the various maps used for efficient item retrieval.
	/// </summary>
	/// <param name="item">The item to remove</param>
	void remove(std::shared_ptr<ITranslatable<T>> item) override
	{
		auto it = std::find(m_items.begin(), m_items.end(), item);
		if (it == m_items.end()) return;

		m_items.erase(it);

		std::pair<int, int> itemGridCell = m_getItemGridCell(item);
		int itemsRemoved = m_integerGrid[itemGridCell].erase(item);
		assert(itemsRemoved == 1);
		if (m_integerGrid[itemGridCell].size() == 0)
			m_integerGrid.erase(itemGridCell);

		itemsRemoved = m_itemCellCache.erase(item);
		assert(itemsRemoved == 1);

		m_setUpdateCellAndNeighbours(itemGridCell);
	}

	/// <summary>
	/// This method is used when an item which already exists in the
	/// DiskGraph has potentially been moved from one cell to another
	/// and repositions it
	/// </summary>
	/// <param name="item">The item whose position has changed</param>
	void updateItem(std::shared_ptr<ITranslatable<T>> item) override
	{
		std::pair<int, int> newGridCell = m_getItemGridCell(item);
		std::pair<int, int> oldGridCell;
		if ((oldGridCell = m_itemCellCache[item]) != newGridCell)
		{
			int itemsRemoved = m_integerGrid[oldGridCell].erase(item);
			assert(itemsRemoved == 1);
			if (m_integerGrid[oldGridCell].size() == 0)
				m_integerGrid.erase(oldGridCell);

			m_integerGrid[newGridCell].insert(item);
			m_itemCellCache[item] = newGridCell;

			m_setUpdateCellAndNeighbours(oldGridCell);
			m_setUpdateCellAndNeighbours(newGridCell);
		}
	}

	/// <summary>
	/// Clears all the lists and maps contaning items.
	/// </summary>
	void clear() override
	{
		m_items.clear();
		m_items.clear();
		m_integerGrid.clear();
		m_itemNeighboursCache.clear();
		m_shouldRecalculateItemNeighboursCache.clear();
		m_itemCellCache.clear();
	}

	/// <summary>
	/// Repositions and sizes the DiskGraph
	/// </summary>
	/// <param name="position">The new position for the DiskGraph</param>
	/// <param name="size">The new size for the DiskGraph</param>
	void resize(const T& position, const T& size) override
	{
		m_position = position;
		m_windowSize = size;
	}

	/// <summary>
	/// This method searches for and returns all the neighbours of the given item. The items that are considered
	/// neighbours are those in the input item's cell and those in the eight neighbouring cells, as well as the item itself.
	/// </summary>
	/// <param name="item">The item whose neighbours are to be returned</param>
	/// <returns>A list of neighbours to the input item</returns>
	std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> item) override
	{
		std::pair<int, int> itemGridCell = m_getItemGridCell(item);

		if (m_shouldRecalculateItemNeighboursCache[itemGridCell])
		{
			m_itemNeighboursCache[itemGridCell].clear();
			for (int y = itemGridCell.second - 1; y <= itemGridCell.second + 1; y++)
			{
				if (y < 0 || y >= m_gridSize.y) continue;
				for (int x = itemGridCell.first - 1; x <= itemGridCell.first + 1; x++)
				{
					if (x < 0 || x >= m_gridSize.x || m_integerGrid[{x, y}].size() == 0) continue;
					std::set<std::shared_ptr<ITranslatable<T>>> itemSetInCell = m_integerGrid[{x, y}];
					for (auto it = itemSetInCell.begin(); it != itemSetInCell.end(); it++)
					{
						m_itemNeighboursCache[itemGridCell].push_back(*it);
					}
				}
			}
			m_shouldRecalculateItemNeighboursCache[itemGridCell] = false;
		}
		return m_itemNeighboursCache[itemGridCell];
	}

	/// <summary>
	/// Draws a representation of the DiskGraph. The first part draws the horizontal and
	/// vertial lines representing the grid. While the second part draws a circle in the middle
	/// of each cell if that cell contains items.
	/// </summary>
	/// <param name="window">The window to render to</param>
	void drawDebug(sf::RenderWindow& window) override
	{
		sf::Vector2u windowSize = window.getSize();
		sf::RectangleShape horizontalLine({ (float)windowSize.x, 1.0f });
		sf::RectangleShape verticalLine({ 1.0f, (float)windowSize.y });
		for (int i = 2 * m_radius; i < windowSize.x; i += 2 * m_radius) {
			verticalLine.setPosition({ (float)i, 0.0f });
			verticalLine.setFillColor(sf::Color::Red);

			window.draw(verticalLine);
		}
		for (int i = 2 * m_radius; i < windowSize.y; i += 2 * m_radius) {
			horizontalLine.setPosition({ 0.0f, (float)i });
			horizontalLine.setFillColor(sf::Color::Red);

			window.draw(horizontalLine);
		}

		sf::CircleShape circle(5.0f);
		circle.setFillColor(sf::Color::Red);

		for (auto gridCellIt = m_integerGrid.begin(); gridCellIt != m_integerGrid.end(); gridCellIt++) {
			if (gridCellIt->second.size() == 0) continue;
			auto gridCell = gridCellIt->first;
			sf::Vector2f circlePosition({
				2 * m_radius * gridCell.first + m_radius - 2.5f,
				2 * m_radius * gridCell.second + m_radius - 2.5f });
			circle.setPosition(circlePosition);
			window.draw(circle);
		}
	}

private:
	/// <summary>
	/// The list of items in the DiskGraph
	/// </summary>
	std::vector<std::shared_ptr<ITranslatable<T>>> m_items;

	/// <summary>
	/// A map from the (x,y) grid-cell coordinates to a list of items in that cell
	/// </summary>
	std::unordered_map<std::pair<int, int>, std::set<std::shared_ptr<ITranslatable<T>>>, PairHash<int, int>> m_integerGrid;

	/// <summary>
	/// A map from the (x,y) grid-cell to a list of items in that cell and the adjacent cells (a list of neighbours for each cell)
	/// </summary>
	std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<ITranslatable<T>>>, PairHash<int, int>> m_itemNeighboursCache;

	/// <summary>
	/// A map from the (x,y) grid-cell to a boolean used to determine whether to update what is in m_itemNeighboursCache before returning it
	/// </summary>
	std::unordered_map<std::pair<int, int>, bool, PairHash<int, int>> m_shouldRecalculateItemNeighboursCache;

	/// <summary>
	/// A map from an item to the (x,y) grid-cell where that item resides
	/// </summary>
	std::unordered_map<std::shared_ptr<ITranslatable<T>>, std::pair<int, int>> m_itemCellCache;
	T m_position;
	T m_windowSize;
	sf::Vector2u m_gridSize;

	/// <summary>
	/// The radius of the items which are stored in the DiskGraph
	/// </summary>
	int m_radius;

	/// <summary>
	/// Returns the cell coordinates which the input item belongs in
	/// </summary>
	/// <param name="item">The item whose cell coordinates are to be found</param>
	/// <returns>The cell coordinates which the input item belongs in</returns>
	inline std::pair<int, int> m_getItemGridCell(std::shared_ptr<ITranslatable<T>> item)
	{
		T position = item->getTranslation();
		return { int(position.x) / (2 * m_radius), int(position.y) / (2 * m_radius) };
	}

	/// <summary>
	/// If an item's position has been updated, then the neighbours cache map has to be updated,
	/// this method sets the "should recalculate item neighbours" flag to true for the input cell and
	/// its eight neighbours.
	/// </summary>
	/// <param name="itemGridCell"></param>
	void m_setUpdateCellAndNeighbours(const std::pair<int, int>& itemGridCell)
	{
		for (int y = itemGridCell.second - 1; y <= itemGridCell.second + 1; y++)
		{
			if (y < 0 || y >= m_gridSize.y) continue;
			for (int x = itemGridCell.first - 1; x <= itemGridCell.first + 1; x++)
			{
				if (x < 0 || x >= m_gridSize.x) continue;
				m_shouldRecalculateItemNeighboursCache[{x, y}] = true;
			}
		}
	}
};

