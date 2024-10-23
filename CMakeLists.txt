cmake_minimum_required(VERSION 3.20)

project(JetsonServer VERSION 1.0.0 LANGUAGES C CXX)

# Set CMake params
set(CMAKE_EXPORT_COMPILE_COMMANDS       ON)
set(CMAKE_CXX_STANDARD                  11)
set(CMAKE_CXX_STANDARD_REQUIRED         ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS    ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /std:c11 /O2 /nologo")
    set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG} /std:c11 /Od /nologo")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp -O3 -mavx2 -mfma -fstrict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG} -fopenmp -mavx2 -mfma -fstrict-aliasing")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm -L/usr/lib -lpthread -ldl")
endif()

# Set Project params
set(CLIENT_NAME clientApp)
set(SERVER_NAME serverApp)
set(LIBRARY_NAME libcode)


# Enable executables
if (MSVC)
    option(CLIENT_MODE "Create the client app. Execute on Windows" ON)
    option(SERVER_MODE "Create the server app. Execute on Jetson Nano" OFF)
else()
    option(CLIENT_MODE "Create the client app. Execute on Windows" OFF)
    option(SERVER_MODE "Create the server app. Execute on Jetson Nano" ON)
endif()


# Find Libraries

## Boost
find_package(Boost REQUIRED COMPONENTS system thread random)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    link_directories(${Boost_LIBRARY_DIRS})
endif()

## OpenCV
set(OpenCV_DIR $ENV{OPENCV_LIB_DIR})
find_package(OpenCV REQUIRED core imgproc highgui imgcodecs)

# Adding files
if(SERVER_MODE)
    add_subdirectory(${LIBRARY_NAME})
    add_subdirectory(serverApp)
endif()

if(CLIENT_MODE)
    add_subdirectory(clientApp)
endif()