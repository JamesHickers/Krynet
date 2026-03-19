cmake_minimum_required(VERSION 3.10)

project(krynet)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_SOURCE_DIR}/app/sciter/include
)

add_library(krynet SHARED
    app/src/main/cpp/main.cpp
)

target_link_libraries(krynet
    ${CMAKE_SOURCE_DIR}/app/sciter/libsciter-android.a
    log
    android
)
