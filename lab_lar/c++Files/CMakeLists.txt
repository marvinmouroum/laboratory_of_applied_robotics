#cmake_minimum_required(VERSION 3.10)
#project(LAR_01)

#set(CMAKE_CXX_STANDARD 11)

#add_executable(LAR_01 main.cpp)


cmake_minimum_required(VERSION 3.5)

project(opencv)



set(OpenCV_DIR ~/opencv/share/OpenCV)
find_package(PkgConfig REQUIRED)
pkg_search_module( TESSERACT REQUIRED tesseract )
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I/usr/include/eigen3 -lbluetooth -lm -pthread")
file(GLOB source_files
        "*.h"
        "*.cpp"
        "Headers/*.hpp"
        "Source/*.cpp"
        "Visualizing/*.cpp"
        "Visualizing/*.hpp"
	"libs/*.c"
	"common/*.c"
        "config/*"
        )

set(SOURCE_FILES main.cpp)
pkg_search_module( TESSERACT REQUIRED tesseract )
add_executable(lar ${source_files}  )



include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(lar ${OpenCV_LIBS} )
target_link_libraries( lar ${TESSERACT_LIBRARIES} )
