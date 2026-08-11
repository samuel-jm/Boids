#include "SFML/System/Vector2.hpp"

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
	T mag = std::sqrt(vec.x * vec.x + vec.y * vec.y);

	T ratio = limit / mag;
	if (ratio < 1)
	{
		vec.x *= ratio;
		vec.y *= ratio;
	}

	return vec;
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