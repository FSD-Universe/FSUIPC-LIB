cmake_minimum_required(VERSION 3.31)
project(fsuipc LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

if (PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(WARNING "The binary directory of CMake cannot be the same as source directory!")
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (MSVC)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32_LEAN_AND_MEAN")
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
        elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /DEBUG")
        endif ()
    else ()
        set(CMAKE_CXX_FLAGS "-fPIC -static -static-libgcc -static-libstdc++ ${CMAKE_CXX_FLAGS}")
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
        elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
        endif ()
    endif ()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "-fPIC -static ${CMAKE_CXX_FLAGS}")
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    endif ()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_CXX_FLAGS "-fPIC -static ${CMAKE_CXX_FLAGS}")
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    endif ()

else ()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif ()

message("BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("BUILD_FLAGS: ${CMAKE_CXX_FLAGS}")
message("PROJECT_NAME: ${PROJECT_NAME}")
message("PROJECT_VERSION: ${PROJECT_VERSION}")
message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

include(file.cmake)

include_directories(src)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILE})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            SUFFIX ".dll"
            PREFIX "lib"
    )
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            SUFFIX ".dylib"
            PREFIX "lib"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
            SUFFIX ".so"
            PREFIX "lib"
    )
endif()
