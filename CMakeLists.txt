cmake_minimum_required(VERSION 2.8)

project( Face-Tracking-Maya )

find_package( OpenCV REQUIRED )

# Include directories
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Generate lists
file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_HEADERS include/*.h)

# Add executable
add_executable( ${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} )

# Link with libraries
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )