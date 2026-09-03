#pragma once

#define _USE_MATH_DEFINES

#include "SFML/System/Vector2.hpp"

#include <string>

/// <summary>
/// This function returns the given Vector2 with its length
/// clamped to the given limit
/// </summary>
/// <typeparam name="T">The type of vector; float, int, or unsigned</typeparam>
/// <param name="vec">The vector to clamp</param>
/// <param name="limit">The value to clamp the vector to</param>
/// <returns>the clamped vector</returns>
template <class T> sf::Vector2<T> limit(sf::Vector2<T> vec, T limit)
{
	return vec.length() <= limit ? vec : vec.normalized() * limit;
}

/// <summary>
/// This struct defines a function call operator and is used
/// as a hash function for whatever maps use a pair as its key
/// </summary>
/// <typeparam name="T">The type of the first pair value</typeparam>
/// <typeparam name="U">The type of the second pair value</typeparam>
template<class T, class U>
struct PairHash
{
	std::size_t operator()(const std::pair<T, U>& pair) const
	{
		return std::hash<T>()(pair.first) ^ std::hash<U>()(pair.second);
	}
};

/// <summary>
/// Takes a pointer to a string and makes it lowercase
/// </summary>
/// <param name="string">The string to modify</param>
void makeLowerCase(std::string* string);