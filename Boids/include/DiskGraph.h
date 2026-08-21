#pragma once

#include <map>
#include <set>
#include <sstream>

#include "SFML/Graphics.hpp"

#include "IPartitioner.h"

template <typename T>
class DiskGraph : public IPartitioner<T>
{
public:
	//void init(std::vector<std::shared_ptr<Boid>> boids, int radius, sf::Vector2u windowSize);
	//void setBoids(std::vector<std::shared_ptr<Boid>> boids, int radius, bool updateGrid = true);
	//void setGridSize(sf::Vector2u windowSize, bool updateGrid = true);
	//std::vector<std::shared_ptr<Boid>> search(const sf::Vector2f& boidPosition);
	//void drawDebug(sf::RenderWindow& window);

	DiskGraph(int radius, const T& position, const T& size) :
		m_radius(radius),
		m_position(position),
		m_windowSize(size),
		m_gridSize(int(size.x) / (2 * radius), int(size.y) / (2 * radius))
	{}

	void insert(std::shared_ptr<ITranslatable<T>> item) override 
	{
		m_items.push_back(item);

		T position = item->getTranslation();
		std::pair<int, int> itemGridCell = m_getItemGridCell(item);
		m_integerGrid[itemGridCell].insert(item);
		m_itemCellCache[item] = itemGridCell;
	}

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
	}

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
		}
	}

	void clear() override 
	{
		m_items.clear();
	}

	void resize(const T& position, const T& size) override 
	{
		m_position = position;
		m_windowSize = size;
	}

	std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> item) override 
	{
		std::pair<int, int> itemGridCell = m_getItemGridCell(item);

		std::vector<std::shared_ptr<ITranslatable<T>>> output;
		for (int y = itemGridCell.second - 1; y <= itemGridCell.second + 1; y++) {
			if (y < 0 || y >= m_gridSize.y) continue;
			for (int x = itemGridCell.first - 1; x <= itemGridCell.first + 1; x++) {
				if (x < 0 || x >= m_gridSize.x || m_integerGrid[{x, y}].size() == 0) continue;
				std::vector<std::shared_ptr<ITranslatable<T>>> nearbyItems;
				for (auto it = m_integerGrid[{x, y}].begin(); it != m_integerGrid[{x, y}].end(); it++) {
					nearbyItems.push_back(*it);
				}
				output.insert(output.end(), nearbyItems.begin(), nearbyItems.end());
			}
		}
		return output;
	}

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

		//std::array line =
		//{
		//	sf::Vertex(),
		//	sf::Vertex()
		//};
		//line[0].color = sf::Color::Green;
		//line[1].color = sf::Color::Green;
		//for (auto item : m_items) {
		//	line[0].position = item->getTranslation();
		//	auto neighbours = search(item);
		//	for (auto neighbour : neighbours) {
		//		line[1].position = neighbour->getTranslation();
		//		window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
		//	}

		// }
	}

private:
	std::vector<std::shared_ptr<ITranslatable<T>>> m_items;
	std::unordered_map<std::pair<int, int>, std::set<std::shared_ptr<ITranslatable<T>>>, PairHash<int, int>> m_integerGrid;
	//std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<ITranslatable<T>>>> m_itemNeighboursCache;
	std::unordered_map<std::shared_ptr<ITranslatable<T>>, std::pair<int, int>> m_itemCellCache;
	T m_position;
	T m_windowSize;
	sf::Vector2u m_gridSize;

	int m_radius;

	inline std::pair<int, int> m_getItemGridCell(std::shared_ptr<ITranslatable<T>> item)
	{
		T position = item->getTranslation();
		return { int(position.x) / (2 * m_radius), int(position.y) / (2 * m_radius) };
	}
};

