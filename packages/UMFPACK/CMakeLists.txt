cmake_minimum_required (VERSION 2.6) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated   -Wno-extra-tokens -Wno-parentheses  -Wno-c++0x-compat -Wc++0x-extensions -ferror-limit=2 -Wno-enum-compare -lstdc++ -Waddress-of-temporary")
set(CMAKE_CXX_COMPILER "/usr/bin/clang")

include_directories(Include Source)
include_directories(../AMD/Include)

file(GLOB src
    "Source/*.c"
)

MESSAGE(STATUS "Source: " ${src} )
add_library(UMFPACK ${src})
