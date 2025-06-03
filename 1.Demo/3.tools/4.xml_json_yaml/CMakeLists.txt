cmake_minimum_required(VERSION 3.14)
project(DataParserDemo C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# vcpkg support
if(DEFINED ENV{VCPKG_ROOT})
    message(STATUS "Using vcpkg from $ENV{VCPKG_ROOT}")
endif()

# Header-only JSON lib
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find packages
find_package(nlohmann_json CONFIG REQUIRED)

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h)
find_library(TINYXML2_LIBRARY NAMES tinyxml2)

if (NOT TINYXML2_INCLUDE_DIR OR NOT TINYXML2_LIBRARY)
    message(FATAL_ERROR "Could not find TinyXML2")
endif()

include_directories(${TINYXML2_INCLUDE_DIR})
set(TINYXML2_LIBRARIES ${TINYXML2_LIBRARY})

find_package(yaml-cpp REQUIRED)

find_path(JANSSON_INCLUDE_DIR jansson.h)
find_library(JANSSON_LIBRARY NAMES jansson)

if(NOT JANSSON_INCLUDE_DIR OR NOT JANSSON_LIBRARY)
    message(FATAL_ERROR "Could not find jansson")
endif()

include_directories(${JANSSON_INCLUDE_DIR})
set(JANSSON_LIBRARIES ${JANSSON_LIBRARY})

find_package(LibXml2 REQUIRED)

add_executable(json_demo_cpp demos/json_demo.cpp)
target_link_libraries(json_demo_cpp PRIVATE nlohmann_json::nlohmann_json)

add_executable(json_demo_c demos/json_demo.c)
target_link_libraries(json_demo_c PRIVATE ${JANSSON_LIBRARIES})

add_executable(xml_demo_cpp demos/xml_demo.cpp)
target_link_libraries(xml_demo_cpp PRIVATE ${TINYXML2_LIBRARIES})

add_executable(xml_demo_c demos/xml_demo.c)
target_include_directories(xml_demo_c PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(xml_demo_c PRIVATE ${LIBXML2_LIBRARIES})

add_executable(yaml_demo_cpp demos/yaml_demo.cpp)
target_link_libraries(yaml_demo_cpp PRIVATE yaml-cpp)

add_executable(yaml_demo_c demos/yaml_demo.c)
target_link_libraries(yaml_demo_c PRIVATE fyaml)
