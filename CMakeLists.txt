cmake_minimum_required(VERSION 3.20)
project(wayhome)

set(CMAKE_CXX_STANDARD 17)


add_executable(wayhome main.cpp timestamp.cpp parser.cpp timestamp.h parser.h)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/cache")
file(MAKE_DIRECTORY ${OUTPUT_DIR}) # making a directory "cache"

include(FetchContent)
set(CPR_ENABLE_SSL OFF)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
GIT_TAG 1.11.0)
FetchContent_MakeAvailable(cpr)


FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)


target_link_libraries(wayhome PRIVATE cpr::cpr nlohmann_json::nlohmann_json)
