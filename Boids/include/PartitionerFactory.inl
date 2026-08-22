#pragma once

#include "Quadtree.h"
#include "DiskGraph.h"
#include "Utilities.h"

#include <memory>
#include <utility>
#include <type_traits>

// Source - https://stackoverflow.com/a/28256800
namespace PartitionerFactory
{
	namespace
	{
		template<typename T, typename... Args>
		std::enable_if_t<std::is_constructible<T, Args...>::value, std::unique_ptr<T>> 
			create(Args... args)
		{
			return std::move(std::make_unique<T>(args...));
		}

		template<typename T, typename... Args>
		std::enable_if_t<!std::is_constructible<T, Args...>::value, std::unique_ptr<T>> 
			create(Args... args)
		{
			return nullptr;
		}
	};

	template<typename T, typename... Args>
	std::unique_ptr<IPartitioner<T>> createPartitioner(std::string partitioner, Args... args)
	{
		makeLowerCase(&partitioner);
		static const std::size_t numArgs = sizeof...(Args);
		if (partitioner == "quadtree")
		{
			return std::move(create<Quadtree<T>>(args...));
		}
		if (partitioner == "diskgraph")
		{
			return std::move(create<DiskGraph<T>>(args...));
		}
	}
};

