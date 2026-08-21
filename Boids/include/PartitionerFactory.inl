#pragma once

#include "Quadtree.h"
#include "DiskGraph.h"
#include "Utilities.h"

#include <memory>
#include <utility>
#include <type_traits>

template<typename T, typename... Args>
class PartitionerFactory
{
public:
	static std::unique_ptr<IPartitioner<T>> createPartitioner(std::string partitioner, Args... args)
	{
		makeLowerCase(&partitioner);
		if (partitioner == "quadtree")
			return create<Quadtree<T>, Args...>(args...);
		if(partitioner == "diskgraph")
			return create<DiskGraph<T>, Args...>(args...);
		
	}
private:
	template<typename T, typename... Args>
	static typename std::enable_if<std::is_constructible<T, Args...>::value, std::unique_ptr<T>>::type
	create(Args... args)
	{
		return std::move(std::make_unique<T>(args...));
	}

	template<typename T, typename... Args>
	static typename std::enable_if<!std::is_constructible<T, Args...>::value, std::unique_ptr<T>>::type
	create(Args... args)
	{
		return nullptr;
	}
};

