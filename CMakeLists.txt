cmake_minimum_required(VERSION 3.16)
project(P2PTransfer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)      # <--- 关键！自动生成 MOC 文件
set(CMAKE_AUTORCC ON)      # 资源文件支持
set(CMAKE_AUTOUIC ON)      # UI 文件支持

find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets Concurrent)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(P2PTransfer WIN32 ${SOURCES} ${HEADERS})

target_include_directories(P2PTransfer PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(P2PTransfer PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Concurrent
)

if(WIN32)
    target_link_libraries(P2PTransfer PRIVATE ws2_32)
endif()