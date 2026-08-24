#pragma once

#include "ITranslatable.h"

/// <summary>
/// This interface is used by classes which intend to efficiently
/// store and retrieve lists of ITranslatable instances. Quadtree
/// and DiskGraph are two concrete implementations
/// </summary>
/// <typeparam name="T">The type which defines a 2D position, such as sf::Vector2f. Must contain .x and .y public members</typeparam>
template<typename T>
class IPartitioner
{
public:
	virtual void insert(std::shared_ptr<ITranslatable<T>> item) = 0;
	virtual void remove(std::shared_ptr<ITranslatable<T>> item) = 0;
	virtual void updateItem(std::shared_ptr<ITranslatable<T>> item) = 0;
	virtual void clear() = 0;
	virtual void resize(const T& position, const T& size) = 0;
	virtual std::vector<std::shared_ptr<ITranslatable<T>>> search(std::shared_ptr<ITranslatable<T>> item) = 0;
	virtual void drawDebug(sf::RenderWindow& window) = 0;
	virtual void pack() {}
};