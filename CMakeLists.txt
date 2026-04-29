cmake_minimum_required(VERSION 4.2)
project(mscript VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

set(MSCRIPT_LIB_SOURCES
    include/mscript/core/Word.hpp

    src/mscript/core/Word.cpp
)

add_library(mscript_core STATIC ${MSCRIPT_LIB_SOURCES})
target_include_directories(mscript_core PUBLIC include)

add_executable(mscript src/main.cpp)
target_link_libraries(mscript PRIVATE mscript_core)
