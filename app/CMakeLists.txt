add_executable(mcc_server main.cc)

target_include_directories(mcc_server PUBLIC ../ext/seasocks/src/main/c)

find_package(Seasocks 1.4.0)

target_link_libraries(mcc_server PRIVATE mcc Seasocks::seasocks)