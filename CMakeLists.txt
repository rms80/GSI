cmake_minimum_required (VERSION 2.6) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated   -Wno-extra-tokens -Wno-parentheses  -Wno-c++0x-compat -Wc++0x-extensions -ferror-limit=2 -Wno-enum-compare -lstdc++ -Waddress-of-temporary")
set(CMAKE_CXX_COMPILER "/usr/bin/clang")

#set(CMAKE_CXX_COMPILER "/usr/bin/gcc-4.4")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -fpermissive   ")


include_directories(Base)
include_directories(packages/include)
include_directories(SparseMatrixSolver)

include_directories(packages/taucs/src)
include_directories(packages/taucs/build/linux64)
include_directories(packages/UMFPACK/Include)

include_directories(packages/UFconfig)
include_directories(packages/AMD/Include)


file(GLOB gsi_src
 "*/*.cpp"
)

MESSAGE(STATUS "foo: ${gsi_src}")

add_library(gsi ${gsi_src})
