cmake_minimum_required(VERSION 3.16)

project(server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCLUDE_PATH "${DLL_PATH}/include")

find_library(libgraphfunctions graphfunctions PATHS ${DLL_PATH})
include_directories(${INCLUDE_PATH})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_executable(server
  main.cpp
  server.h
  server.cpp
)

target_link_libraries(server Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    ${libgraphfunctions})

include(GNUInstallDirs)
install(TARGETS server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
