cmake_minimum_required(VERSION 3.3)
project(Prototype)

set(COMPILE_FLAGS "-pedantic -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c90 -O3")

#include_directories(/Library/Frameworks/SDL2.framework/Headers)
#link_directories(/Library/Frameworks/SDL2.framework)
#Above lines are used to help find_library find the files if the default lookup path yields nothing

find_library(math m)
find_library(sdl SDL2)
#Finds m and SDL2 from user's own frameworks folder

#MESSAGE(WARNING PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})

set(SOURCE_FILES
    src/AI.c
    src/AI.h
    src/game.c
    src/game.h
    src/game_objects.c
    src/game_objects.h
    src/generic.c
    src/generic.h
    src/init.c
    src/init.h
    src/main.c
    src/main.h
    src/UI.c
    src/UI.h
    src/world_generation.c
    src/world_generation.h)

set(ASSETS_DIR assets)
set(AI_DIR ${ASSETS_DIR}/ai)
set(IMAGES_DIR ${ASSETS_DIR}/images)

set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin_mac)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})


file(COPY ${IMAGES_DIR} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${AI_DIR} DESTINATION ${PROJECT_BINARY_DIR})

#configure_file("${images}" ${PROJECT_BINARY_DIR} COPYONLY)

add_executable(Prototype ${SOURCE_FILES})
target_link_libraries(Prototype ${sdl} ${math})