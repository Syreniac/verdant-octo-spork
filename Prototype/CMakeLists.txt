cmake_minimum_required(VERSION 3.3)
project(Prototype)
project(mytest)

set(COMPILE_FLAGS "-pedantic -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c90 -O3")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
include_directories(/Library/Frameworks/SDL2_image.framework/Headers)
link_directories(/Library/Frameworks/SDL2_image.framework)
include_directories(/Library/Frameworks/SDL2_ttf.framework/Headers)
link_directories(/Library/Frameworks/SDL2_ttf.framework)
#find_package(SDL2_mixer REQUIRED)
include_directories(/Library/Frameworks/SDL2_mixer.framework/Headers)
link_directories(/Library/Frameworks/SDL2_mixer.framework)
#include_directories(/Library/Frameworks/SDL_mixer.framework/Headers)
#link_directories(/Library/Frameworks/SDL_mixer.framework)
#Above lines are used to help find_library find the files if the default lookup path yields nothing
#Ie. if you install the framework but don't reboot CLion or the system before building the executable.

find_library(math m)
find_library(sdl SDL2)
find_library(sdlimg SDL2_image)
find_library(sdlttf SDL2_TTF)
#find_library(sdlmixer SDL2_mixer)
#find_library(sdlmixer SDL_mixer)
#if find_library fails to locate the library folder, just pass the .framework folder directly into target_link_libraries.
#Finds m and SDL2 from user's own frameworks folder

#MESSAGE(WARNING PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})

set(COMMON_FILES
        src/AI.c
        src/AI.h
        src/audio.c
        src/audio.h
        src/controls.c
        src/controls.h
        src/game.c
        src/game.h
        src/game_objects.c
        src/game_objects.h
        src/generic.c
        src/generic.h
        src/graphics.c
        src/graphics.h
        src/init.c
        src/init.h
        src/UI.c
        src/UI.h
        src/world_generation.c
        src/world_generation.h
        src/announcements.c
        src/announcements.h)
set(SOURCE_FILES
    src/main.c
    src/main.h
    ${COMMON_FILES})
set(TEST_SOURCE_FILES src/all.test.c src/all.test.h ${COMMON_FILES})

set(ASSETS_DIR assets)
set(AI_DIR ${ASSETS_DIR})
set(IMAGES_DIR ${ASSETS_DIR}/images)
file(COPY ${PROJECT_SOURCE_DIR}/${IMAGES_DIR} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/${AI_DIR} DESTINATION ${PROJECT_BINARY_DIR})
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

add_executable(Prototype ${SOURCE_FILES})
add_executable(mytest ${TEST_SOURCE_FILES})
add_library(libcommon ${COMMON_FILES})

add_custom_target(copy-runtime-files
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/${ASSETS_DIR} ${PROJECT_BINARY_DIR})
add_dependencies(libcommon copy-runtime-files)
add_dependencies(Prototype libcommon)
add_dependencies(mytest libcommon)


target_link_libraries(libcommon ${sdl} ${sdlimg} ${sdlttf} ${math} /Library/Frameworks/SDL2_mixer.framework)
target_link_libraries(Prototype libcommon)
target_link_libraries(mytest check libcommon)