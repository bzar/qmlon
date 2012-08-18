CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(qmlon)

file(GLOB SOURCES src/*.cpp)
file(GLOB TEST_SOURCES test/*.cpp)

list(APPEND CMAKE_CXX_FLAGS -std=c++0x)
include_directories(include)

add_library(qmlon ${SOURCES})

add_executable(test_spritesheet ${TEST_SOURCES})
target_link_libraries(test_spritesheet qmlon)

install(TARGETS qmlon DESTINATION lib)
install(DIRECTORY include DESTINATION include)

file(COPY test/sprite.sheet DESTINATION .)

