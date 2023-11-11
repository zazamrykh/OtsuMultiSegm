cmake_minimum_required(VERSION 3.24)
project(OtsuSegm)

set(CMAKE_CXX_STANDARD 17)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(OtsuSegm main.cpp
        otsu_segm.h
        otsu_segm.c
)

target_link_libraries( OtsuSegm ${OpenCV_LIBS} )
