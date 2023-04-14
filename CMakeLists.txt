cmake_minimum_required(VERSION 3.12)

# Set a name and a version number for your project:
project(mcc VERSION 0.0.1 LANGUAGES CXX)

add_library(mcc INTERFACE)
target_include_directories(mcc INTERFACE include)
target_compile_features(mcc INTERFACE cxx_std_20)

# compile the tests
# add_subdirectory(tests)

# compile the application
add_subdirectory(apps)
