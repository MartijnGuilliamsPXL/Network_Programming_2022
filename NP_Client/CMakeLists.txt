cmake_minimum_required(VERSION 3.14)

project(NP_Client LANGUAGES CXX)

add_compile_definitions(ZMQ_STATIC)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)


include_directories(
        Sources/
        Sources/ZeroMQ/include/
)

link_directories(
        Sources/ZeroMQ/lib/
)

set(project_headers
    Sources/service.h
)

set(project_sources
        Sources/main.cpp
        Sources/service.cpp
)

set(ALL_LIBS
        general
        libzmq.a
        ws2_32
        iphlpapi
        Qt6::Core
        Qt6::Concurrent
)

add_executable (${PROJECT_NAME} ${project_sources} ${project_headers})

target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})
