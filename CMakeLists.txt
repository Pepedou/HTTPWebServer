cmake_minimum_required (VERSION 2.8)
project (HTTPWebServer)
find_package(Threads)
add_executable(WebServer main.c main.c listener.h listener.c connections.h connections.c worker.h worker.c httpParser.h httpParser.c signalHandler.c)
set(CMAKE_BUILD_TYPE Debug)
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})
