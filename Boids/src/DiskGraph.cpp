//#include "DiskGraph.h"
//
//void DiskGraph::init(std::vector<std::shared_ptr<Boid>> boids, int radius, sf::Vector2u windowSize)
//{
//	setBoids(boids, radius, false);
//	setGridSize(windowSize);
//}
//
//void DiskGraph::setBoids(std::vector<std::shared_ptr<Boid>> boids, int radius, bool updateGrid) {
//	m_boids = boids;
//	m_radius = radius;
//	if(updateGrid)
//		m_populateGrid();
//}
//
//void DiskGraph::setGridSize(sf::Vector2u windowSize, bool updateGrid) {
//	m_gridSize = { unsigned(ceil(windowSize.x / float(2 * m_radius))), unsigned(ceil(windowSize.y / float(2 * m_radius))) };
//	if (updateGrid)
//		m_populateGrid();
//}
//
//std::vector<std::shared_ptr<Boid>> DiskGraph::search(const sf::Vector2f& boidPosition)
//{
//	sf::Vector2i gridCell = { int(boidPosition.x / (2 * m_radius)), int(boidPosition.y / (2 * m_radius)) };
//
//	std::vector<std::shared_ptr<Boid>> output;
//	for (int y = gridCell.y - 1; y <= gridCell.y + 1; y++) {
//		if (y < 0 || y >= m_gridSize.y) continue;
//		for (int x = gridCell.x - 1; x <= gridCell.x + 1; x++) {
//			if (x < 0 || x >= m_gridSize.x || m_integerGrid[{x, y}].size() == 0) continue;
//			std::vector<std::shared_ptr<Boid>> boidSet;
//			for (auto it = m_integerGrid[{x,y}].begin(); it != m_integerGrid[{x,y}].end(); it++) {
//				boidSet.push_back(*it);
//			}
//			output.insert(output.end(), boidSet.begin(), boidSet.end());
//		}
//	}
//	return output;
//}
//
//void DiskGraph::drawDebug(sf::RenderWindow& window)
//{
//	sf::Vector2u windowSize = window.getSize();
//	sf::RectangleShape horizontalLine({(float)windowSize.x, 1.0f});
//	sf::RectangleShape verticalLine({1.0f, (float)windowSize.y });
//	for (int i = 2 * m_radius; i < windowSize.x; i += 2 * m_radius) {
//		verticalLine.setPosition({ (float)i, 0.0f });
//		verticalLine.setFillColor(sf::Color::Red);
//
//		window.draw(verticalLine);
//	}
//	for (int i = 2 * m_radius; i < windowSize.y; i += 2 * m_radius) {
//		horizontalLine.setPosition({0.0f, (float)i});
//		horizontalLine.setFillColor(sf::Color::Red);
//
//		window.draw(horizontalLine);
//	}
//
//	sf::CircleShape circle(5.0f);
//	circle.setFillColor(sf::Color::Red);
//
//	for (auto gridCellIt = m_integerGrid.begin(); gridCellIt != m_integerGrid.end(); gridCellIt++) {
//		auto gridCell = gridCellIt->first;
//		sf::Vector2f circlePosition({
//			2 * m_radius * gridCell.first + m_radius - 2.5f,
//			2 * m_radius * gridCell.second + m_radius - 2.5f });
//		circle.setPosition(circlePosition);
//		window.draw(circle);
//	}
//
//	std::array line =
//	{
//		sf::Vertex(),
//		sf::Vertex()
//	};
//	line[0].color = sf::Color::Green;
//	line[1].color = sf::Color::Green;
//	for (auto boid : m_boids) {
//		line[0].position = boid->getTranslation();
//		auto neighbours = search(boid->getTranslation());
//		for (auto neighbour : neighbours) {
//			line[1].position = neighbour->getTranslation();
//			window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
//		}
//		
//	}
//}
//
//void DiskGraph::m_populateGrid() {
//	m_integerGrid.clear();
//	for (auto boid : m_boids) {
//		sf::Vector2f position = boid->getTranslation();
//		m_integerGrid[{int(position.x) / (2 * m_radius), int(position.y) / (2 * m_radius)}].insert(boid);
//	}
//}
