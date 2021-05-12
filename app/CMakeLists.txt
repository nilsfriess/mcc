add_executable(mcc_server main.cc)

target_include_directories(mcc_server PUBLIC ../ext/seasocks/src/main/c)

target_link_libraries(mcc_server PRIVATE mcc seasocks)