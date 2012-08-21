CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(qmlon)

file(GLOB SOURCES src/*.cpp)

list(APPEND CMAKE_CXX_FLAGS -std=c++0x)
include_directories(include)

add_library(qmlon ${SOURCES})

add_executable(test_spritesheet test/spritesheet.cpp)
target_link_libraries(test_spritesheet qmlon)

add_executable(test_schema test/schema.cpp)
target_link_libraries(test_schema qmlon)

install(TARGETS qmlon DESTINATION lib)
install(DIRECTORY include DESTINATION include)

file(COPY test/spritesheet.qmlon DESTINATION .)
file(COPY schema/schema.qmlon DESTINATION .)
file(COPY schema/spritesheet-schema.qmlon DESTINATION .)
