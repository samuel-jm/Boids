cmake_minimum_required(VERSION 3.28)
project(Boids LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external)

add_subdirectory(external)

add_executable(boids)
target_sources(boids
	PRIVATE
		src/Boid.cpp
		src/Boids.cpp
		src/BoidType.cpp
		src/Main.cpp
		src/Utilities.cpp
	
	PRIVATE
		FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/Boid.h
			include/Boids.h
			include/BoidType.h
			include/DiskGraph.h
			include/IPartitioner.h
			include/ITranslatable.h
			include/PartitionerFactory.inl
			include/Quadtree.h
			include/Slider.inl
			include/Utilities.h
)

target_compile_features(boids PRIVATE cxx_std_17)
target_link_libraries(boids PRIVATE externalLibrary)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT boids)

add_custom_command(
	TARGET boids 
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
	${CMAKE_SOURCE_DIR}/resources
	$<TARGET_FILE_DIR:boids>/resources
)