#pragma once

#define _USE_MATH_DEFINES

#include "SFML/System/Vector2.hpp"

#include <math.h>
#include <string>

template <class T> int sign(T val)
{
	return val < 0 ? -1 : 1;
}

template <class T> sf::Vector2<T> reciprocal(sf::Vector2<T> val)
{
	return sf::Vector2<T>(val.y, val.x);
}

template <class T> sf::Vector2<T> limit(sf::Vector2<T> vec, T limit)
{
	return vec.length() <= limit ? vec : vec.normalized() * limit;
}

template <class T> sf::Vector2<T> normalise(sf::Vector2<T> vec)
{
	T mag = std::sqrt(vec.x * vec.x + vec.y * vec.y);

	if (mag == 0) return vec;

	T ratio = (T)1 / mag;

	vec.x *= ratio;
	vec.y *= ratio;

	return vec;
}

/// <summary>
/// Return an <c>Angle</c> representing the angle between <c>vec</c> and
/// the positive X axis.
/// </summary>
/// <typeparam name="T">The type of the vector</typeparam>
/// <param name="vec">The vector</param>
/// <returns>the <c>Angle</c></returns>
template<class T> sf::Angle angle(sf::Vector2<T> vec)
{
	float add = vec.y < 0 ? 2 * M_PI : 0;
	float scale = vec.y < 0 ? -1 : 1;
	return sf::Angle(sf::radians(scale * acos(vec.normalized().x) + add));
}

template<class T, class U>
struct PairHash
{
	std::size_t operator()(const std::pair<T, U>& pair) const
	{
		return std::hash<T>()(pair.first) ^ std::hash<U>()(pair.second);
	}
};

void makeLowerCase(std::string* string);