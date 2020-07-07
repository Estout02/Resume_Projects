cmake_minimum_required(VERSION 3.15)
project(Calculator)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")

#asan
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#coverage
set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
#coverage
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
SET(GCC_COVERAGE_COMPILE_FLAGS "-fno-omit-frame-pointer")
SET(GCC_COVERAGE_COMPILE_FLAGS "-mno-omit-leaf-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")


include_directories(.)

add_executable(msdscript
        cont.cpp
        cont.hpp
        env.cpp
        env.hpp
        expr.cpp
        expr.hpp
        main.cpp
        parse.cpp
        parse.hpp
        pointer.hpp
        step.cpp
        step.hpp
        value.cpp
        value.hpp)
add_library(msdscriptl
        cont.cpp
        cont.hpp
        env.cpp
        env.hpp
        expr.cpp
        expr.hpp
        parse.cpp
        parse.hpp
        pointer.hpp
        step.cpp
        step.hpp
        value.cpp
        value.hpp)