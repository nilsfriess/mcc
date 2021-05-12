add_library(mcc mcc.cc)
target_include_directories(mcc PUBLIC ../include)
# require at least c++11
target_compile_features(mcc PUBLIC cxx_std_14)
