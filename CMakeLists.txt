cmake_minimum_required(VERSION 3.15)
project(stock-quote-alert)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Força o runtime estático do MSVC (/MT em vez de /MD)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)

add_executable(stock-quote-alert main.cpp)
target_compile_definitions(stock-quote-alert PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(stock-quote-alert PRIVATE cpr::cpr nlohmann_json::nlohmann_json CURL::libcurl)