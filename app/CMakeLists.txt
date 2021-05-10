add_executable(mcc_server main.cc)
target_link_libraries(mcc_server PRIVATE mcc seasocks)