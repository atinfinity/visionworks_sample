cmake_minimum_required(VERSION 2.8)
project(immediate_vs_graph)

find_package(VisionWorks REQUIRED)
find_package(OpenCV REQUIRED)

file(GLOB SOURCES "*.cpp" "*.c" "*.h" "*.hpp")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native")
endif()

if(VisionWorks_FOUND)
	if(OpenCV_FOUND)
		include_directories(${VisionWorks_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
		add_executable(${PROJECT_NAME} ${SOURCES})
		target_link_libraries(${PROJECT_NAME} ${VisionWorks_LIBRARIES} ${OpenCV_LIBS})
	endif(OpenCV_FOUND)
endif(VisionWorks_FOUND)

