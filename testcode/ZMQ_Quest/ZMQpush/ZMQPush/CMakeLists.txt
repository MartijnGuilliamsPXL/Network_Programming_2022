cmake_minimum_required (VERSION 3.18.3)

project(ZMQPush)

add_compile_definitions(ZMQ_STATIC)

include_directories(
	Sources/
	External/ZMQ/include/
)

link_directories(
	External/ZMQ/lib/
)

set(project_headers

)

set(project_sources
	Sources/main.cpp
)

set(ALL_LIBS
	libzmq.a
	ws2_32
	iphlpapi
)

add_executable (${PROJECT_NAME} ${project_sources} ${project_headers})

target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})