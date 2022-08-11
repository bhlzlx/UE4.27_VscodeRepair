cmake_minimum_required(VERSION 3.18)

project(VscodeRepair)

set(RAPIDJSON_INCLUDE "${CMAKE_SOURCE_DIR}/rapidjson/include")

add_subdirectory(jsonutility/src)

add_executable(VscodeRepair)

target_sources(VscodeRepair
PRIVATE
    main.cpp
)

target_include_directories(VscodeRepair
PRIVATE
    ${CMAKE_SOURCE_DIR}/rapidjson/include
)

target_link_libraries(VscodeRepair
PRIVATE
    GxJsonUtility
)