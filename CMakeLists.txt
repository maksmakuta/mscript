cmake_minimum_required(VERSION 4.2)
project(mscript VERSION 0.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

set(MSCRIPT_LIB_SOURCES
    include/mscript/core/AST.hpp
    include/mscript/core/Word.hpp
    include/mscript/frontend/Token.hpp
    include/mscript/frontend/Lexer.hpp
    include/mscript/frontend/Parser.hpp
    include/mscript/frontend/Printer.hpp
    include/mscript/backend/Environment.hpp
    include/mscript/backend/Interpreter.hpp
    include/mscript/backend/Value.hpp
    include/mscript/backend/NativeFunctions.hpp

    src/mscript/core/Word.cpp
    src/mscript/frontend/Token.cpp
    src/mscript/frontend/Lexer.cpp
    src/mscript/frontend/Parser.cpp
    src/mscript/frontend/Printer.cpp
    src/mscript/backend/Environment.cpp
    src/mscript/backend/Interpreter.cpp
    src/mscript/backend/Value.cpp
    src/mscript/backend/NativeFunctions.cpp
)

add_library(mscript_core STATIC ${MSCRIPT_LIB_SOURCES})
target_include_directories(mscript_core PUBLIC include)

add_executable(mscript src/main.cpp)
target_link_libraries(mscript PRIVATE mscript_core)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
        tests/test_lexer.cpp
)

add_executable(mscript_tests ${TEST_SOURCES})
target_link_libraries(mscript_tests PRIVATE mscript_core gtest_main)

include(CTest)
add_test(NAME LexerTests COMMAND mscript_tests)
