# Boids

Boids is a C++ application created using SFML for running a [Boids](https://en.wikipedia.org/wiki/Boids) simulation.

## Screenshots

![Boids](screenshots/Boids_DiskGraph.jpg)
![Boids](screenshots/Boids_Quadtree.jpg)

## TODO

- Update UI to allow for greater user flexibility, including setting and disabling debug mode
- Use CMake to allow for more flexibility in how/where the project may be built

## Requirements

- C++17
- SFML V3

## How to Use

1. Clone the repository
1. Download and unzip [SFML](https://www.sfml-dev.org/download/)
1. Move the lib and include directories of SFML into Boids/Boids/dependencies/SFML
1. Open the project in Visual Studio
1. Build the project

The bin/ directory of SFML contains dlls necessary to run the project, Place:
- sfml-graphics-3.dll, sfml-system-3.dll, sfml-window-3.dll in Boids/Release or
- sfml-graphics-d-3.dll, sfml-system-d-3.dll, sfml-window-d-3.dll in Boids/Debug

depending on if you built the Release or Debug configurations respectively.

You can now run the project from Visual Studio

## License

[MIT](https://choosealicense.com/licenses/mit/)
