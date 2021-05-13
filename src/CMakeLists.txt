add_library(mcc mcc.cc board2darray.cc piece.cc)
target_include_directories(mcc PUBLIC ../include)
target_compile_features(mcc PUBLIC cxx_std_17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
