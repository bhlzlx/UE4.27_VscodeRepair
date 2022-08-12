cmake_minimum_required(VERSION 3.18)

project(VscodeRepair)

add_executable(VscodeRepair)

target_sources(VscodeRepair
PRIVATE
    main.cpp
)

target_include_directories(VscodeRepair
PRIVATE
    ${CMAKE_SOURCE_DIR}/rapidjson/include
)