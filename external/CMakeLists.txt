add_library(externalLibrary INTERFACE)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external")

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
    FIND_PACKAGE_ARGS COMPONENTS Graphics Window System
)

FetchContent_MakeAvailable(SFML)

target_link_libraries(externalLibrary INTERFACE SFML::Graphics SFML::Window SFML::System)