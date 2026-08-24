#pragma once

/// <summary>
/// This interface is used by classes which can have a 2D position and is
/// necessarily implemented by classes which intend to be partitioned by
/// IPartitioner classes
/// </summary>
/// <typeparam name="T">The type which defines a 2D position, such as sf::Vector2f. Must contain .x and .y public members</typeparam>
template<typename T>
class ITranslatable
{
public:
	virtual const T& getTranslation() = 0;
	virtual void setTranslation(const T& translation) = 0;
};