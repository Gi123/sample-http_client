cmake_minimum_required(VERSION 3.10)

project(HttpClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${PROJECT_SOURCE_DIR}/app/include())

add_executable(HttpClient
                app/src/main.cpp
                app/src/http_client.cpp)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

target_include_directories(HttpClient PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIRS})
target_link_libraries(HttpClient PRIVATE Boost::boost OpenSSL::SSL OpenSSL::Crypto)