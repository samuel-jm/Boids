#pragma once

#include "Quadtree.h"
#include "DiskGraph.h"
#include "Utilities.h"

#include <memory>
#include <utility>
#include <type_traits>

/// <summary>
/// This class implements the factory method pattern and is used to produce concrete instances of IPartitioner<T>
/// without the caller needing to know the details of these concrete implementations.
/// This solution was inspired by the following StackOverflow post: https://stackoverflow.com/a/28256800
/// </summary>
class PartitionerFactory
{
public:
	template<typename T, typename... Args>
	static std::unique_ptr<IPartitioner<T>> createPartitioner(std::string partitioner, Args... args)
	{
		makeLowerCase(&partitioner);
		if (partitioner == "quadtree")
			return std::move(create<Quadtree<T>>(args...));
		if (partitioner == "diskgraph")
			return std::move(create<DiskGraph<T>>(args...));
	}
private:
	template<typename T, typename... Args>
	static std::enable_if_t<std::is_constructible<T, Args...>::value, std::unique_ptr<T>>
		create(Args... args)
	{
		return std::move(std::make_unique<T>(args...));
	}

	template<typename T, typename... Args>
	static std::enable_if_t<!std::is_constructible<T, Args...>::value, std::unique_ptr<T>>
		create(Args... args)
	{
		return nullptr;
	}
};